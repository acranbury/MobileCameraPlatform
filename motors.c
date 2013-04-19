/* Motor functions */
#include <mc9s12c32.h>
#include "utils.h"
#include "pwm.h"
#include "timer.h"
#include "motors.h"
#include "encoders.h"

/* Control Law defines*/
                    
// Default gain values
#define P_GAIN_DEF  600
#define I_GAIN_DEF  100 

// Minimum and Maximum drive values for PWM for motors to actuallly move.
#define MAXDRIVE   100   
// Motor drive value scaling.
#define DRIVE_SCALE_VAL         1000000

// Gain Divisor
#define GAIN_DIV   1000

// Compile time option to choose between 24V and 12V supply. 
#define TWELVEVOLT
//#define TWENTYFOURVOLT

#ifdef  TWELVEVOLT   

#define MINDRIVE   55
#define MINPERIOD  800                             // Period in micro seconds.
#define MAXPERIOD  1800 
#define MINFREQ    1250
#define MAXFREQ    555
#define BVALUE     293
           
#elif   TWENTYFOURVOLT

#define MINDRIVE   40 
#define MINPERIOD  370                             // Period in micro seconds.
#define MAXPERIOD  800
#define MINFREQ    2702
#define MAXFREQ    1250
#define BVALUE     -282

#endif

// Limits for error that remain in the range of reality.
#define MAX_SPEED_ERROR   MAXDRIVE - MINDRIVE
#define SPEED_SET_SCALE  100                      // Used to adjust speed setpoint.


static long set_point_1 = 0;                       // Current setpoint, 0% - 100%
static long set_point_2 = 0; 
static long P_GAIN  = P_GAIN_DEF; 
static long I_GAIN  = I_GAIN_DEF; 

/* Initialize motor PWM channels */
void motor_init(void) {
    
    // Set prescaler for source clock
    SET_PWM_PRESCALE_A(PWM_PRESCALE_64);
    
    // Set channel polarity
    PWM_POL(MOTOR1_PWM,PWM_POL_HI);
    PWM_POL(MOTOR2_PWM,PWM_POL_HI);
    
    // Set clock source
    PWM_CLK(MOTOR1_PWM,PWM_CLK_A);
    PWM_CLK(MOTOR2_PWM,PWM_CLK_A);
    
    // Set alignment
    PWM_LEFT_ALIGN(MOTOR1_PWM);
    PWM_LEFT_ALIGN(MOTOR2_PWM);
    
    // Period and duty cycles
    PWM_PER(MOTOR1_PWM) = MOTOR_PER_DEF;
    PWM_DTY(MOTOR1_PWM) = 0;
    PWM_PER(MOTOR2_PWM) = MOTOR_PER_DEF;
    PWM_DTY(MOTOR2_PWM) = 0;
    
    SET_BITS(MOTOR_PORT_DDR,MOTOR_BITS);    // Set motor lines as outputs
    MOTOR1_DIRA = MOTOR_FW;
    MOTOR1_DIRB = (MOTOR1_DIRA == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
    //motor_set_direction(MOTOR1, MOTOR_FW);  // Set initial motor direction    <-- can't use motor_set_direction for initial direction setting
    
    // Enable both PWM channels
    PWM_ENABLE(MOTOR1_PWM);
    PWM_ENABLE(MOTOR2_PWM);
    
    // Enable timer module if not already enabled
    if(!(TSCR1 & TSCR1_TEN_MASK)) EnableTimer;     
    
    TC_OC(TC_MOTOR);  // Channel set to output compare
    SET_OC_ACTION(TC_MOTOR,OC_OFF);   // Keep output line turned off.
    TC(TC_MOTOR) = TCNT + MOTOR_CNTL_LAW_DELTA;    // Preset OC channel
    
    TC_INT_ENABLE(TC_MOTOR);  // Enable timer channel interrupts
}

/* Set motor PWM period */
void motor_set_period(byte motor, byte period) {
    switch(motor) {
    case MOTOR1:
        PWM_PER(MOTOR1_PWM) = period;
        break;
    case MOTOR2:
        PWM_PER(MOTOR2_PWM) = period;
        break;
    default:
        break;
    }
}

/* Set motor PWM duty cycle */
void motor_set_duty(byte motor, byte duty) {
    switch(motor) {
    case MOTOR1:
        PWM_DTY(MOTOR1_PWM) = duty;
        break;
    case MOTOR2:
        PWM_DTY(MOTOR2_PWM) = duty;
        break;
    default:
        break;
    }
}

/* Set motor direction */
void motor_set_direction(byte motor, byte direction) {
    switch(motor) {
    case MOTOR1:
        if(MOTOR1_DIRA != direction) {
            // Turn off motors first
            MOTOR1_DIRA = 0;
            MOTOR1_DIRB = 0;
            msleep(10);     // Wait for motors to come to a stop
            
            MOTOR1_DIRA = direction;
            MOTOR1_DIRB = (direction == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
        }
        break;
    case MOTOR2:
        if(MOTOR2_DIRA != direction) {
            // Turn off motors first
            MOTOR2_DIRA = 0;
            MOTOR2_DIRB = 0;
            msleep(10);     // Wait for motors to come to a stop
            
            MOTOR2_DIRA = direction;
            MOTOR2_DIRB = (direction == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
        }
        break;
    default:
        break;
    }
}

/* Set motor speed */
void motor_set_speed(byte motor, char speed) {
    switch(motor) {
    case MOTOR1C:
        set_point_1 = ((speed * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
        break;
    case MOTOR2C:
        set_point_2 = ((speed * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the right motor.
        break;
    default:
        break;
    }
}

/*****************************************************************************/
interrupt VECTOR_NUM(TC_VECTOR(TC_MOTOR)) void control_law_ISR(void) {
    static long integral_error_1 = 0, integral_error_2 = 0;
    static int last_cntl_out_1 = 0, last_cntl_out_2 = 0;
    
    long speed_error_1 = 0, speed_error_2 = 0;
    long read_period_1 = 0, read_period_2 = 0;
    char drive_value_1 = 0, drive_value_2 = 0;
    
    // Calculate error.     
    read_period_1 = ((DRIVE_SCALE_VAL/encoder_period(1)) + (BVALUE)); 
    read_period_2 = ((DRIVE_SCALE_VAL/encoder_period(2)) + (BVALUE));
    read_period_1 *= (MAXDRIVE - MINDRIVE);
    read_period_1 /= (MINFREQ - MAXFREQ);
    read_period_2 *= (MAXDRIVE - MINDRIVE);
    read_period_2 /= (MINFREQ - MAXFREQ);
    speed_error_1 = set_point_1 - read_period_1;
    speed_error_2 = set_point_2 - read_period_2;
    
    // Check that error is in a realistic range.    
    if ((speed_error_1 < MAX_SPEED_ERROR) && (speed_error_1 > -MAX_SPEED_ERROR)) {  
        // Check if integration needs to be performed (not at rails). 
        if ((PWM_DTY(MOTOR1_PWM) > MINDRIVE) && (PWM_DTY(MOTOR1_PWM) < MAXDRIVE))
          integral_error_1 += speed_error_1; 
        
        // Perform control law calculations
        drive_value_1 = (((speed_error_1 * P_GAIN) + (integral_error_1 * I_GAIN))/GAIN_DIV) + MINDRIVE;
        
        if (drive_value_1 > MAXDRIVE)
          drive_value_1 = MAXDRIVE;
        else if (drive_value_1 < MINDRIVE)
          drive_value_1 = MINDRIVE;
        
        // Save last drive value.
        last_cntl_out_1 = (drive_value_1);
        // Write new drive value to PWM hardware.
        PWM_DTY(MOTOR1_PWM) = drive_value_1;
    }
    
    // Check that error is in a realistic range.    
    if ((speed_error_2 < MAX_SPEED_ERROR) && (speed_error_2 > -MAX_SPEED_ERROR)) {  
        // Check if integration needs to be performed (not at rails). 
        if ((PWM_DTY(MOTOR1_PWM) > MINDRIVE) && (PWM_DTY(MOTOR1_PWM) > MINDRIVE))
          integral_error_2 += speed_error_2; 
        
        // Perform control law calculations
        drive_value_2 = (((speed_error_2 * P_GAIN) + (integral_error_2 * I_GAIN))/GAIN_DIV) + MINDRIVE;
        
        if (drive_value_2 > MAXDRIVE)
          drive_value_2 = MAXDRIVE;
        else if (drive_value_2 < MINDRIVE)
          drive_value_2 = MINDRIVE;
        
        // Save last drive value.
        last_cntl_out_2 = (drive_value_2);
        // Write new drive value to PWM hardware.
        PWM_DTY(MOTOR1_PWM) = drive_value_2;
    }
    
    
    TC(TC_MOTOR) = TCNT + MOTOR_CNTL_LAW_DELTA;   // Set up interrupt to run again.
    
}
