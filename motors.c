/* Motor functions */
#include <mc9s12c32.h>
#include "utils.h"
#include "pwm.h"
#include "timer.h"
#include "encoders.h"
#include "motors.h"


static int set_point_1 = 0;         // Current setpoint, 0% - 100%
static int set_point_2 = 0; 
static int P_GAIN = P_GAIN_DEF; 
static int I_GAIN = I_GAIN_DEF;


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
    MOTOR2_DIRA = MOTOR_FW;
    MOTOR2_DIRB = (MOTOR2_DIRA == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
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
    static char curDir_1 = MOTOR_FW, curDir_2 = MOTOR_FW; 
    
    switch(motor) {
    case MOTOR1C:
        if (speed == 0)
            set_point_1 = 0;
        else if ((speed < 0) && (curDir_1 == MOTOR_FW)) {      // If the robot is moving forward and the command says to reverse, 
            motor_set_direction (MOTOR1, MOTOR_RV);      // change direction.
            set_point_1 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir_1 = MOTOR_RV;                          // Set current motor direction.
        }
        else if ((speed > 0) && (curDir_1 == MOTOR_RV)) {     // If the robot is moving forward and the command says to reverse, 
            motor_set_direction (MOTOR1, MOTOR_FW);      // change direction.
            set_point_1 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir_1 = MOTOR_FW;                          // Set current motor direction.
        }
        else 
            set_point_1 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
        break;
    case MOTOR2C:
        if (speed == 0)
            set_point_2= 0;
        else if ((speed < 0) && (curDir_2 == MOTOR_FW)) {     // If the robot is moving forward and the command says to reverse, 
            motor_set_direction (MOTOR2, MOTOR_RV);      // change direction.
            set_point_2 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir_2 = MOTOR_RV;                          // Set current motor direction.
        }
        else if ((speed > 0) && (curDir_2 == MOTOR_RV)) {     // If the robot is moving forward and the command says to reverse, 
            motor_set_direction (MOTOR2, MOTOR_FW);      // change direction.
            set_point_2 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir_2 = MOTOR_FW;                          // Set current motor direction.
        }
        else 
            set_point_2 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
        break;
    default:
        break;
    }
}

// Return absolute value.
int abs(int num) {
    if (num < 0)
        return (num * -1);
    else
        return num;
}

/*****************************************************************************/
interrupt VECTOR_NUM(TC_VECTOR(TC_MOTOR)) void control_law_ISR(void) {
    static long integral_error_1 = 0, integral_error_2 = 0;
    static long last_cntl_out_1 = 0, last_cntl_out_2 = 0;
    
    long speed_error_1 = 0, speed_error_2 = 0;
    volatile long read_period_1 = 0, read_period_2 = 0;
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
        if ((!((PWM_DTY(MOTOR1_PWM) <= MINDRIVE) && (speed_error_1 < 0))) && (!((PWM_DTY(MOTOR1_PWM) >= MAXDRIVE) && (speed_error_1 > 0))))
          integral_error_1 += speed_error_1; 
        
        // Perform control law calculations
        drive_value_1 = (((speed_error_1 * P_GAIN) + (integral_error_1 * I_GAIN))/GAIN_DIV) + MINDRIVE;
        
        if (drive_value_1 > MAXDRIVE)
          drive_value_1 = MAXDRIVE;
        else if ((drive_value_1 < MINDRIVE) & (set_point_1 == 0))
          drive_value_1 = 0;
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
        if ((!((PWM_DTY(MOTOR2_PWM) <= MINDRIVE) && (speed_error_2 < 0))) && (!((PWM_DTY(MOTOR2_PWM) >= MAXDRIVE) && (speed_error_2 > 0))))
          integral_error_2 += speed_error_2; 
        
        // Perform control law calculations
        drive_value_2 = (((speed_error_2 * P_GAIN) + (integral_error_2 * I_GAIN))/GAIN_DIV) + MINDRIVE;
        
        if (drive_value_2 > MAXDRIVE)
          drive_value_2 = MAXDRIVE;
        else if ((drive_value_2 < MINDRIVE) & (set_point_2 == 0))
          drive_value_2 = 0;
        else if (drive_value_2 < MINDRIVE)
          drive_value_2 = MINDRIVE;
        
        // Save last drive value.
        last_cntl_out_2 = (drive_value_2);
        // Write new drive value to PWM hardware.
        PWM_DTY(MOTOR2_PWM) = drive_value_2;
    }
    
    
    TC(TC_MOTOR) = TCNT + MOTOR_CNTL_LAW_DELTA;   // Set up interrupt to run again.
    
}
