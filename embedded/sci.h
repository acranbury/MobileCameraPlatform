/* SCI module macros */
#ifndef _SCI_H
#define _SCI_H

#include "utils.h"


#define SCI_BUFSIZ          40  // Size in bytes of the SCI receive ring buffer (sending up to five 8byte commands using 38400baud)
#define SCI_CMDSIZ          8   // Length of command in bytes

// No parity, 8 data bits, 1 stop bit (mask bits cleared)
#define SCICR1_INIT     ( SCICR1_PE_MASK | SCICR1_M_MASK )

/*****************************************************************************/

// SCI baud macros (8MHz bus clock)
/* 38400 baud is the highest stable baud rate usable
 SCIBD Value | Desired Rate | Actual Rate | Difference | Notes
-------------|--------------|-------------|------------|-------
    4545     |    110       |   110.01    |  +0.01%    |
    1666     |    300       |   300.12    |  +0.04%    |
    417      |    1200      |   1199.04   |  -0.08%    |
    208      |    2400      |   2403.85   |  +0.16%    |
    104      |    4800      |   4807.69   |  +0.16%    |
    52       |    9600      |   9615.38   |  +0.16%    |
    26       |    19200     |   19230.77  |  +0.16%    |
    13       |    38400     |   38461.54  |  +0.16%    |
    9        |    57600     |   55555.55  |  -3.55%    | Send and receive works with termite, unsure how reliable it is with high noise
    4        |    115200    |   125000.00 |  +8.51%    | Baud rate too skewed to send from HCS12, receiving to HCS12 works
*/
#define SET_SCI_BAUD(value) (SCIBD = (value))
#define BAUD110             4545
#define BAUD300             1666
#define BAUD1200            417
#define BAUD2400            208
#define BAUD4800            104
#define BAUD9600            52
#define BAUD19200           26
#define BAUD38400           13
#define BAUD57600           9


// SCI interrupt macros
#define SCI_RX_INT_ENABLE   SET_BITS(SCICR2,SCICR2_RIE_MASK)    // Enable SCI receive interrupt
#define SCI_RX_INT_DISABLE  CLR_BITS(SCICR2,SCICR2_RIE_MASK)    // Disable SCI receive interrupt
#define SCI_TX_INT_ENABLE   SET_BITS(SCICR2,SCICR2_SCTIE_MASK)  // Enable SCI transmit interrupt
#define SCI_TX_INT_DISABLE  CLR_BITS(SCICR2,SCICR2_SCTIE_MASK)  // Disable SCI transmit interrupt

/*****************************************************************************/

void SCIinit(void);
void SCIflush(void);
void SCIdequeue(char *);
void SCIputc(char);
void SCIputs(const char *);
void SCIprintf(const char *, ... );
char SCIgetc(void);
char *SCIgets(void);


#endif // _SCI_H