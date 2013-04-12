/* Joystick test */

#include <hidef.h>      /* common defines and macros */
#include <stdlib.h>     // atoi()
#include <string.h>     // memset() and strcmp()
#include "derivative.h" /* derivative-specific definitions */
#include "timer.h"
#include "lcd.h"
#include "sci.h"
#include "servo.h"
#include "stepper.h"


#define CMD_LEN     3   // command size
#define PNG         1
#define ABT         2
#define TXT         3
#define PAN         4
#define TLT         5


void cmdparser(char *);
int cmdconv(char *);
void seekcmd(char *, int *);

/*****************************************************************************/

#pragma MESSAGE DISABLE C1420   // Function call result ignored warning disable (for memset)
void main(void) {
    char buffer[SCI_BUFSIZ+1] = {0};
    
    // initialize timer, SCI, servo, stepper and LCD modules
    timer_init();
    SCIinit();
    servo_init();
    stepper_init();
    
    msleep(16);
    LCDinit();
    
    EnableInterrupts;
    
    LCDputs("Calibrating...");
    stepper_calibrate();
    
    LCDclear();
    LCDputs("Ready.");
    SCIputs("HCS12 ready to go!");
    
    for(;;) {
        SCIdequeue(buffer);
        cmdparser(buffer);
        memset(buffer, 0, SCI_BUFSIZ+1);
    } /* loop forever */
}

/***********************cmdparser*******************************
* 
*   Purpose: Parse the command string to call the correct function. 
*
*   Input: char *tempcmd: input command string.
*
*   Output: int result: Resulting integer value.
*
***************************************************************/
void cmdparser(char *buffer) {
    char tempcmd[CMD_LEN+1] = {0};
    int numchars = 0;
    static int numcmd = 0;  
    
    // while we do not have a valid command and we still have characters to check,
    while(buffer[0] && ((numchars+2) < SCI_BUFSIZ)) { 
        tempcmd[0] = buffer[numchars];
        tempcmd[1] = buffer[numchars+1]; 
        tempcmd[2] = buffer[numchars+2]; 
        tempcmd[3] = 0;
        
        switch(cmdconv(tempcmd)) {
        case 0:     // If no command found, go to next character.
            seekcmd(buffer, &numchars);
            break;
        
        case PNG:   // ping
            SCIprintf("png%d",numcmd);   // echo command confirmation with stamp.
            LCDclear();
            LCDputs("Ping!");
            
            numcmd++;
            numchars += 8;
            break;
        
        case ABT:  // STOP THE PRESS!
            SCIprintf("abt%d",numcmd);
            LCDclear();
            LCDputs("Abort!\nAbort!");
            
            numcmd++;
            numchars += 8;
            break;
        
        case TLT:  // Tilt the camera.
            SCIprintf("tlt%d",numcmd);
            LCDprintf("\rServo Angle: %d", atoi(&(buffer[4])));
            servo_set_angle(atoi(&(buffer[4])));
            
            numcmd++;
            numchars += 8;
            break;
        
        case PAN:  // Tilt the camera.
            SCIprintf("pan%d",numcmd);
            LCDprintf("\nStep angle: %d", atoi(&(buffer[4])));
            stepper_set_angle(atoi(&(buffer[4])));
            
            numcmd++;
            numchars += 8;
            break;
        
        /*
        case TXT:  // Print to LCD.
            SCIprintf("txt%d",numcmd);
            numcmd++;
            LCDprintf("%s", buffer+3);
            numchars += 3;
            break;
        */
        
        }
    }
}

/***********************cmdconv*********************************
* 
*   Purpose: Convert cmd string to a usable integer value.
*
*   Input: char *cmd: input command string.
*
*   Output: int result: Resulting integer value.
*
***************************************************************/
int cmdconv(char *cmd) {
    if(!(strcmp(cmd, "png")))
        return PNG;
    else if(!(strcmp(cmd, "abt")))
        return ABT;
    else if(!(strcmp(cmd, "tlt")))
        return TLT;  
    else if(!(strcmp(cmd, "pan")))
        return PAN;   
    else if(!(strcmp(cmd, "txt")))
        return TXT;
    else
        return 0;
}

/***********************seekcmd********************************
* 
*   Purpose: Seek through buffer until a zero is found. 
*
*   Input: char *buffer
*
***************************************************************/
void seekcmd(char *buffer, int *numchars) {
    while ((*buffer++ != 0) && (*numchars + 2 <= SCI_BUFSIZ))   // Seek until first zero.      
        (*numchars)++;
    while ((*buffer++ == 0) && (*numchars + 2 <= SCI_BUFSIZ))   // Seek until non-zero is found. (beginning of new cmd).
        (*numchars)++;
}