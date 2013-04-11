
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#include "timer.h"
#include "sci.h"
#include "lcd.h"
#include <string.h>     // memset() and strcmp()


// command size
#define CMD_LEN 3
#define PNG 1
#define ABT 2
#define TXT 3
#define PAN 4
#define TLT 5

void cmdparser(char *);
int cmdconv(char *);
void seekcmd(char *, int *);


#pragma MESSAGE DISABLE C1420   // Function call result ignored warning disable (for memset)
void main(void) {
    char buffer[SCI_BUFSIZ+1] = {0};
    
    // variables to test for successful case resolution
    //char tilt, pancam, abort, err = 0;
    
    // initialize timer, LCD, and SCI modules
    timer_init();
    SCIinit();
    
    msleep(16);
    LCDinit();
    
    EnableInterrupts;
    
    // print to the screen
    LCDprintf("Hello World");
    
    // grab chars from SCI and echo them back
    for(;;){
        SCIdequeue(buffer);
        cmdparser(buffer);
        memset(buffer, 0, SCI_BUFSIZ+1);
    }
}

/***********************cmdparser*******************************
* 
*   Purpose: Parse the command string to call the correct function. 
*
*   Input: char * tempcmd: input command string.
*
*   Output: int result: Resulting integer value.
*
***************************************************************/
void cmdparser (char * buffer) {
    char tempcmd [CMD_LEN + 1] = {0};
    int numchars = 0;
    static int numcmd = 0;  
    
    // while we do not have a valid command and we still have characters to check,
    while ((buffer[0]) && ((numchars + 2) < SCI_BUFSIZ )) { 
        tempcmd [0] = buffer[numchars];
        tempcmd [1] = buffer[1 + numchars]; 
        tempcmd [2] = buffer[2 + numchars]; 
        tempcmd [3] = 0;  		  
        switch (cmdconv(tempcmd)) {
        case 0:   // If no command found, go to next character.
            seekcmd(buffer, &numchars);
            break;
            
          case PNG:   // ping
          SCIprintf("png%d",numcmd);   // echo command confirmation with stamp.
          numcmd++;
          numchars+=8;
          break;
          
          case ABT:  // STOP THE PRESS!
          SCIprintf("abt%d",numcmd);
          numcmd++;
          numchars+=8;
          //__asm ("STOP");
          break;
          
          case TLT:  // Tilt the camera.
          SCIprintf("tlt%d",numcmd);
          numcmd++;
          numchars+=8;
          break;
          
          case PAN:  // Tilt the camera.
          SCIprintf("pan%d",numcmd);
          numcmd++;
          numchars+=8;
          break;  	    
                           
          /*case TXT:  // Print to LCD.
          SCIprintf("txt%d",numcmd);
          numcmd++;
          LCDprintf("%s", buffer+3);
          numchars+=3;
          break;  */
          
        }
    }
}

/***********************cmdconv*********************************
* 
*   Purpose: Convert cmd string to a usable integer value.
*
*   Input: char * tempcmd: input command string.
*
*   Output: int result: Resulting integer value.
*
***************************************************************/
int cmdconv (char * tempcmd) {
    if (!(strcmp(tempcmd, "png")))
        return PNG;
    else if (!(strcmp(tempcmd, "abt")))
        return ABT;
    else if (!(strcmp(tempcmd, "tlt")))
        return TLT;  
    else if (!(strcmp(tempcmd, "pan")))
        return PAN;   
    else if (!(strcmp(tempcmd, "txt")))
        return TXT;
    else
        return 0;
}

/***********************seekcmd********************************
* 
*   Purpose: Seek through buffer until a zero is found. 
*
*   Input: char * buffer
*
***************************************************************/
void seekcmd (char * buffer, int * numchars) {
    while ((*buffer++ != 0) && (*numchars + 2 <= SCI_BUFSIZ) )  // Seek until first zero.      
        (*numchars)++;
    while ((*buffer++ == 0) && (*numchars + 2 <= SCI_BUFSIZ) ) // Seek until non-zero is found. (beginning of new cmd).
        (*numchars)++;
}