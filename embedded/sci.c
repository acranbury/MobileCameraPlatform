/* SCI module functions */

#include <hidef.h>      // required for EnableInterrupts and DisableInterrupts macros
#include <mc9s12c32.h>
#include <stdio.h>      // for vsprintf()
#include <stdarg.h>     // for va_*() functions
#include "utils.h"
#include "sci.h"


// Global SCI Receive ring buffer
char SCIbuf[SCI_BUFSIZ];
byte SCIcount;

/* Initialize SCI module */
void SCIinit(void) {
    SET_SCI_BAUD(BAUD38400);        // Set baud rate
    CLR_BITS(SCICR1,SCICR1_INIT);   // Set port configuration
    
    SCIflush();
    SCI_RX_INT_ENABLE;  // Enable SCI receive interrupt
    
    SET_BITS(SCICR2,( SCICR2_RE_MASK | SCICR2_TE_MASK ));   // Enable transmitter and receiver
    
    SCIcount = 0;   // Clear SCI received bytes counter
}

/* Flush SCI ring buffer */
void SCIflush(void) {
    byte i;
    for(i=0; i < SCI_BUFSIZ; i++)
        SCIbuf[i] = '\0';
}

/* Empty internal SCI buffer */
void SCIdequeue(char *buf){
    byte bytenum;
    static byte i = 0;
    
    // Dequeue only if buffer contains enough bytes for a full command
    if (SCIcount >= SCI_CMDSIZ) {
        // Loop through all bytes in command
        for (bytenum=0; bytenum < SCI_CMDSIZ; bytenum++) {
        DisableInterrupts;
          *buf++ = SCIbuf[i];   // Fill buffer with bytes from SCI buffer
          SCIbuf[i] = '\0';
          i = (byte)(((word)i+1) % SCI_BUFSIZ); // Increment to next index; wrap to beginning if buffer overflows
          SCIcount--;           // Decrement global buffer byte counter
        EnableInterrupts;
        }
    }
}

/* Write char to SCI */
void SCIputc(char c) {
    while(!(SCISR1 & SCISR1_TDRE_MASK));    // Wait for transmit register to become empty
    SCIDRL = c;
}

/* Write string to SCI */
void SCIputs(const char *str) {
    while(*str)
        SCIputc(*str++);
}

#pragma MESSAGE DISABLE C1420   // Function call result ignored warning disable (for vsprintf)
/* Write formatted string to SCI */
void SCIprintf(const char *fmt, ... ) {
    char buffer[SCI_BUFSIZ];
    
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    
    SCIputs(buffer);
}

/* Read char from SCI */
char SCIgetc(void) {
    char c;
    
    while(!(SCISR1 & SCISR1_RDRF_MASK));    // Wait for receive register to become empty
    c = SCIDRL;
    
    // Catch escape sequences
    switch(c) {
    case '~':   /* clear screen */
        c = '\a';
        break;
    case '`':   /* move cursor to beginning */
        c = '\r';
        break;
    case '\\':  /* move cursor to second line */
        c = '\n';
        break;
    case '|':   /* delete one char */
        c = '\b';
        break;
    default:
        break;
    }
    return c;
}

/* Read string from SCI */
/* NOTE: not working, probably not needed if using ISR for SCI receieve */
char *SCIgets(void) {
    char *str = NULL;
    while(!(SCISR1 & SCISR1_IDLE_MASK))     // Get chars unitl receieve line goes idle
        *(str++) = SCIgetc();
    return str;
}

/*****************************************************************************/

/* SCI interrupt handler */
interrupt VectorNumber_Vsci void SCI_RX_ISR(void) {
    static byte i = 0;
    
    // Check error flags before grabbing chars
    if(!(SCISR1 & ( SCISR1_OR | SCISR1_NF | SCISR1_FE | SCISR1_PF ))) {
        SCIbuf[i] = SCIDRL;     // Store received byte into ring buffer
        i = (byte)(((word)i+1) % SCI_BUFSIZ);   // Increment to next index; wrap to beginning if buffer overflows
        if(SCIcount < SCI_BUFSIZ)
            SCIcount++;         // Increment global buffer byte counter
    } else {
        (void)SCIDRL; // Clear flags by reading data register
    }
}
