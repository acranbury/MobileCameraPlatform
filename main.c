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
#include "motors.h"
#include "encoders.h"

#define TC_HEARTBEAT    6           // Heartbeat timer channel
#define HEARTBEAT       1000000     // Heartbeat count in microseconds (with timer prescaler of 8)

#define CMD_LEN     3   // command size
#define PNG         1
#define ABT         2
#define RES         3
#define TXT         4
#define PAN         5
#define TLT         6
#define HOM         7
#define CAL         8
#define MOV         9


word volatile heartbeat_count;


void cmdparser(char *);
int cmdconv(char *);
void seekcmd(char *, int *);
void stop_motion(void);
void start_motion(void);
void start_heartbeat(void);
 
/*****************************************************************************/

#pragma MESSAGE DISABLE C1420   // Disable "Function call result ignored" warning (for memset)
void main(void) {
    char buffer[SCI_BUFSIZ+1] = {0};
    
    // Initialize all nessesary modules
    timer_init();
    SCIinit();
    servo_init();
    stepper_init();
    encoder_init();
    motor_init();
    msleep(16);
    LCDinit();
    //start_heartbeat();
    
    EnableInterrupts;
    
    motor_set_speed(MOTOR1C, 0);
    motor_set_speed(MOTOR2C, 0);
    
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
            SCIprintf("png%05d",numcmd);   // echo command confirmation with stamp.
            //LCDclear();
            //LCDputs("Ping!");
            
            numcmd++;
            numchars += 8;
            break;
        
        case ABT:  // STOP THE PRESS!
            SCIprintf("abt%05d",numcmd);
            LCDclear();
            LCDputs("Abort!\nAbort!");
            stop_motion();
            
            numcmd++;
            numchars += 8;
            break;
        
        case RES:  // Resume operation
            SCIprintf("res%05d",numcmd);
            LCDclear();
            LCDputs("Resuming...");
            start_motion();
            LCDclear();
            LCDputs("Resumed");
            
            numcmd++;
            numchars += 8;
            break;
        
        #pragma MESSAGE DISABLE C2705   // Disable "Possible loss of data" warning (for atoi)
        case TLT:  // Tilt camera up/down
            SCIprintf("tlt%05d",numcmd);
            LCDprintf("\rServo Angle: %03d", atoi(&(buffer[4])));
            servo_set_angle(atoi(&(buffer[4])));
            
            numcmd++;
            numchars += 8;
            break;
        
        case PAN:  // Pan camera left/right
            SCIprintf("pan%05d",numcmd);
            LCDprintf("\nStep Angle:  %03d", atoi(&(buffer[4])));
            stepper_set_angle(atoi(&(buffer[4])));
            
            numcmd++;
            numchars += 8;
            break;
        
        case HOM:  // Home the camera
            SCIprintf("hom%05d",numcmd);
            servo_set_angle(90);
            stepper_set_angle(90);
            LCDclear();
            LCDputs("Camera homed");
            
            numcmd++;
            numchars += 8;
            break;
        
        case CAL:  // Calibrate the camera
            SCIprintf("hom%05d",numcmd);
            
            LCDputs("Calibrating...");
            stepper_calibrate();
            servo_set_angle(90);
            stepper_set_angle(90);
            LCDclear();
            LCDputs("Calibrated");
            
            numcmd++;
            numchars += 8;
            break;
        
        case MOV:
            SCIprintf("mov%05d", numcmd);
            LCDprintf("Motor %c: %3d\n", buffer[numchars+3], atoi(&buffer[numchars + 4]));
            motor_set_speed(buffer[numchars+3], (char)(atoi(&buffer[numchars + 4])));
            
            numcmd++;
            numchars += 8;
            break;
            
        /*
        case TXT:  // Print to LCD.
            SCIprintf("txt%05d",numcmd);
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
    else if(!(strcmp(cmd, "res")))
        return RES;
    else if(!(strcmp(cmd, "tlt")))
        return TLT;  
    else if(!(strcmp(cmd, "pan")))
        return PAN;   
    else if(!(strcmp(cmd, "txt")))
        return TXT;
    else if(!(strcmp(cmd, "hom")))
        return HOM;
    else if(!(strcmp(cmd, "cal")))
        return CAL;    
    else if(!(strcmp(cmd, "mov")))
        return MOV;
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

/* Turn off all motion */
void stop_motion(void) {
    // Turn off motors
    motor_set_speed(MOTOR1C, 0);
    motor_set_speed(MOTOR2C, 0);
    msleep(20);                 // Wait for motors to come to a stop
    TC_INT_DISABLE(TC_MOTOR);   // Disable motor control law
    
    servo_set_angle(90);        // Move servo to center
    TC_INT_DISABLE(TC_STEPPER); // Disable stepper control
}

/* Start up motion again */
void start_motion(void) {
    TC_INT_ENABLE(TC_MOTOR);    // Re-enable motor control law
    TC_INT_ENABLE(TC_STEPPER);  // Re-enable stepper control
}

/* Start heartbeat timer */
void start_heartbeat(void) {
    TC_OC(TC_HEARTBEAT);
    SET_OC_ACTION(TC_HEARTBEAT,OC_OFF);
    
    TC(TC_HEARTBEAT) = TCNT + HEARTBEAT;    // Preset OC channel
    TC_INT_ENABLE(TC_HEARTBEAT);            // Enable timer channel interrupt
}


/* Heartbeat interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_HEARTBEAT)) void heartbeat_ISR(void) {
    static word prev_count = 0;
    word cur_count;
    
    cur_count = TC(TC_HEARTBEAT);   // Acknowledge interrupt by accessing timer channel
    
    // Check if correct number of overflows has occurred
    if(get_overflow_count() == 15) { //HEARTBEAT / 0xFFFF) {
        TC(TC_HEARTBEAT) += HEARTBEAT;  // Rearm channel register, clearing TFLG as well
        heartbeat_count++;
    }
    
    prev_count = cur_count;     // Remember count for next time
}
