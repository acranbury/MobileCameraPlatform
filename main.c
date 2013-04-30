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
#define DST         10
#define SPN         11
#define AIM         12


word volatile heartbeat_count;

void cmdparser(char *);
int cmdconv(char *);
void seekcmd(char *, int *);
void stop_motion(void);
void start_motion(void);
void start_heartbeat(void);
 
/*****************************************************************************/

#pragma MESSAGE DISABLE C1420   // Disable "Function call result ignored" warning (for memset)
#pragma MESSAGE DISABLE C2705   // Disable "Possible loss of data" warning (for heartbeat TCNT)
#pragma MESSAGE DISABLE C12056  // Disable "SP debug info incorrect because of optimization" warning
void main(void) {
    char buffer[SCI_BUFSIZ+1] = {0};
    
    // Initialize all nessesary modules
    timer_init();
    SCIinit();
    servo_init();
    stepper_init();
    encoder_init();
    motor_init();
    msleep(16); LCDinit();
    //start_heartbeat();    // Not used, TCNT overflow interrupts causing issues
    
    DDRP |= PTP_PTP0_MASK;  // Set DDR for laser GPIO
    
    // Motors off initially
    motor_set_speed(MOTOR1C, 0);
    motor_set_speed(MOTOR2C, 0);
    
    EnableInterrupts;
    
    LCDputs("Calibrating...");
    stepper_calibrate();
    
    LCDclear(); LCDputs("Ready.");
    SCIputs("HCS12 ready to go!");
    
    for(;;) {
        SCIdequeue(buffer);
        cmdparser(buffer);
        memset(buffer, 0, SCI_BUFSIZ+1);    // Clear out the command buffer after each command parsed
        //LCDclear(); LCDprintf("\r%7d  %7d\n%7d  %7d", drive_value1, drive_value2, speed_error1, speed_error2);
    } /* loop forever */
}

/***********************cmdparser*******************************
* 
*   Purpose: Parse the command string to call the correct function. 
*
*   Input: char *cmdtype: input command string.
*
*   Output: int result: Resulting integer value.
*
***************************************************************/
void cmdparser(char *buffer) {
    char cmdtype[CMD_LEN+1] = {0};
    int numchars = 0;
    static int numcmd = 0;  // Count of number of commands parsed
    static byte tog = 0;    // Laser toggle bit
    char motor1_pct, motor2_pct;
    
    
    cmdtype[0] = buffer[numchars];
    cmdtype[1] = buffer[numchars+1]; 
    cmdtype[2] = buffer[numchars+2]; 
    cmdtype[CMD_LEN] = '\0';    // Terminate input command after three bytes, leaving just the command type
    
    switch(cmdconv(cmdtype)) {
    case 0:     // If no command found, go to next character.
        seekcmd(buffer, &numchars);
        break;
    
    case PNG:   // ping
        SCIprintf("png%05d",numcmd);   // echo command confirmation with stamp.
        //LCDclear(); LCDputs("Ping!");
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case ABT:  // STOP THE PRESS!
        SCIprintf("abt%05d",numcmd);
        LCDclear(); LCDputs("Abort!\nAbort!");
        stop_motion();
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case RES:  // Resume operation
        SCIprintf("res%05d",numcmd);
        LCDclear(); LCDputs("Resuming...");
        start_motion();
        LCDclear(); LCDputs("Resumed");
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case TLT:  // Tilt camera up/down
        SCIprintf("tlt%05d",numcmd);
        LCDprintf("\rServo Angle: %03d", atoi(&buffer[4]));
        servo_set_angle((byte)atoi(&buffer[4]));
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case PAN:  // Pan camera left/right
        SCIprintf("pan%05d",numcmd);
        LCDprintf("\nStep Angle:  %03d", atoi(&buffer[4]));
        stepper_set_angle((byte)atoi(&buffer[4]));
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case HOM:  // Home the camera
        SCIprintf("hom%05d",numcmd);
        servo_set_angle(90);
        stepper_set_angle(90);
        LCDclear(); LCDputs("Camera homed");
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case CAL:  // Calibrate the camera
        SCIprintf("hom%05d",numcmd);
        
        LCDclear(); LCDputs("Calibrating...");
        
        stepper_calibrate();
        servo_set_angle(90);
        stepper_set_angle(90);
        
        LCDclear(); LCDputs("Calibrated");
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case MOV:   // Set motor speed (0% - 100%)
        SCIprintf("mov%05d", numcmd);
        if(buffer[numchars+3] == '2') {
            // Both motors selected
            TC_INT_DISABLE(TC_MOTOR);   // Disable motor control law
              motor_set_speed(MOTOR1C, (char)atoi(&buffer[numchars+4]));
              motor_set_speed(MOTOR2C, (char)atoi(&buffer[numchars+4]));
            TC_INT_ENABLE(TC_MOTOR);    // Re-enable motor control law
            LCDclear(); LCDprintf("\rM%c: %3d  M%c: %3d", MOTOR1C, atoi(&buffer[numchars+4]), MOTOR2C, atoi(&buffer[numchars+4]));
        }
        else {
            motor_set_speed(buffer[numchars+3], (char)atoi(&buffer[numchars+4]));
            LCDclear(); LCDprintf("\rMotor %c: %3d", buffer[numchars+3], atoi(&buffer[numchars+4]));
        }
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case DST:   // Set motor distance (+speed)
        SCIprintf("dst%05d", numcmd);
        switch(buffer[numchars+4]) {
        case '0':   // Setting a speed
            
            motor1_pct = motor_convert(MOTOR1C, (int)atoi(&buffer[numchars+5]));
            motor2_pct = motor_convert(MOTOR2C, (int)atoi(&buffer[numchars+5]));
            
            // Set speed to both motors if 4th char is a '2'
            if(buffer[numchars+3] == '2') {
                TC_INT_DISABLE(TC_MOTOR);   // Disable motor control law
                    motor_set_speed(MOTOR1C, motor1_pct);
                    motor_set_speed(MOTOR2C, motor2_pct);
                TC_INT_ENABLE(TC_MOTOR);    // Re-enable motor control law
            } else
                motor_set_speed(buffer[numchars+3],
                  motor_convert(buffer[numchars+3], (int)atoi(&buffer[numchars+5]))
                );
            
            //LCDclear(); LCDprintf("\rM1: %3d M2: %3d", motor1_pct, motor2_pct);
            //LCDprintf("\nS1: %3d S2: %3d", atoi(&buffer[numchars+5]), atoi(&buffer[numchars+5]));
            
            break;
        case '1':   // Setting a distance
            
            // Set speed to both motors if 4th char is a '2'
            if(buffer[numchars+3] == '2') {
                motor_set_distance(MOTOR1C, (word)atoi(&buffer[numchars+5]));
                motor_set_distance(MOTOR2C, (word)atoi(&buffer[numchars+5]));
                //LCDclear(); LCDprintf("\nD%c: %3d  D%c: %3d", MOTOR1C, atoi(&buffer[numchars+5]), MOTOR2C, atoi(&buffer[numchars+5]));
            }
            else {
                motor_set_distance(buffer[numchars+3], (word)atoi(&buffer[numchars+5]));
                //LCDclear(); LCDprintf("\rDist %c: %3d", buffer[numchars+3], atoi(&buffer[numchars+5]));
            }
            
            break;
        }
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case SPN:   // Spin in place
        SCIprintf("spn%05d", numcmd);
        DisableInterrupts;
        motor_set_speed(MOTOR1C, -50);
        motor_set_speed(MOTOR2C, 50);
        motor_set_distance(MOTOR1C, (word)atoi(&buffer[numchars+3]));
        motor_set_distance(MOTOR2C, (word)atoi(&buffer[numchars+3]));
        EnableInterrupts;
        SCIprintf("Dist: %3d\n", atoi(&buffer[numchars+3]));
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    case AIM:   // Toggle laser pointer
        SCIprintf("aim%05d",numcmd);
        tog = (tog) ? 0 : 1;
        PTP_PTP0 = (tog) ? 1 : 0;
        
        numcmd++;
        numchars += SCI_CMDSIZ;
        break;
    
    /*
    case TXT:   // Print to LCD
        SCIprintf("txt%05d",numcmd);
        LCDprintf("%s", buffer+3);
        numcmd++;
        numchars += 3;
        break;
    */
    
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
    else if(!(strcmp(cmd, "dst")))
        return DST;
    else if(!(strcmp(cmd, "spn")))
        return SPN;
    else if(!(strcmp(cmd, "aim")))
        return AIM;
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
