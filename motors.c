/* Motor functions */

#include <mc9s12c32.h>
#include "utils.h"
#include "timer.h"
#include "pwm.h"
#include "encoders.h"
#include "motors.h"


static int P_GAIN = P_GAIN_DEF;
static int I_GAIN = I_GAIN_DEF;
static word set_point1 = 0, set_point2 = 0;         // Current motor setpoint (0% - 100%)
//static byte volatile count_down = 25;


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
    
    // Set initial direction
    MOTOR1_DIRA = MOTOR_FW;
    MOTOR1_DIRB = (MOTOR1_DIRA == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
    MOTOR2_DIRA = MOTOR_FW;
    MOTOR2_DIRB = (MOTOR2_DIRA == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
    
    // Enable both PWM channels
    PWM_ENABLE(MOTOR1_PWM);
    PWM_ENABLE(MOTOR2_PWM);
    
    // Enable timer module if not already enabled
    if(!(TSCR1 & TSCR1_TEN_MASK)) EnableTimer;     
    
    TC_OC(TC_MOTOR);                // Channel set to output compare
    SET_OC_ACTION(TC_MOTOR,OC_OFF); // Keep output line turned off, we are only using the timer channel
    TC(TC_MOTOR) = TCNT + MOTOR_CNTL_LAW_DELTA;     // Preset OC channel
    
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
            msleep(20);     // Wait for motors to come to a stop
            
            MOTOR1_DIRA = direction;
            MOTOR1_DIRB = (direction == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
        }
        break;
    case MOTOR2:
        if(MOTOR2_DIRA != direction) {
            // Turn off motors first
            MOTOR2_DIRA = 0;
            MOTOR2_DIRB = 0;
            msleep(20);     // Wait for motors to come to a stop
            
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
    static byte curDir1 = MOTOR_FW, curDir2 = MOTOR_FW;
    
    switch(motor) {
    case MOTOR1C:
        if (speed == 0)
            set_point1 = 0;
        else if ((speed < 0) && (curDir1 == MOTOR_FW)) {      // If the robot is moving forward and the command says to reverse, 
            //motor_set_direction(MOTOR1, MOTOR_RV);  // change direction
            MOTOR1_DIRA = MOTOR_RV;
            MOTOR1_DIRB = MOTOR_FW;
            
            set_point1 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir1 = MOTOR_RV;                    // Remember last motor direction
        }
        else if ((speed > 0) && (curDir1 == MOTOR_RV)) {     // If the robot is moving forward and the command says to reverse, 
            //motor_set_direction(MOTOR1, MOTOR_FW);  // change direction
            MOTOR1_DIRA = MOTOR_FW;
            MOTOR1_DIRB = MOTOR_RV;
            
            set_point1 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir1 = MOTOR_FW;                    // Remember last motor direction
        }
        else
            set_point1 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
        break;
    case MOTOR2C:
        if (speed == 0)
            set_point2= 0;
        else if ((speed < 0) && (curDir2 == MOTOR_FW)) {     // If the robot is moving forward and the command says to reverse, 
            //motor_set_direction (MOTOR2, MOTOR_RV);      // change direction.
            MOTOR2_DIRA = MOTOR_RV;
            MOTOR2_DIRB = MOTOR_FW;
            
            set_point2 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir2 = MOTOR_RV;                          // Set current motor direction.
        }
        else if ((speed > 0) && (curDir2 == MOTOR_RV)) {     // If the robot is moving forward and the command says to reverse, 
            //motor_set_direction (MOTOR2, MOTOR_FW);      // change direction.
            MOTOR2_DIRA = MOTOR_FW;
            MOTOR2_DIRB = MOTOR_RV;
            
            set_point2 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
            curDir2 = MOTOR_FW;                          // Set current motor direction.
        }
        else
            set_point2 = ((abs(speed) * (MAXDRIVE - MINDRIVE)) / SPEED_SET_SCALE) + MINDRIVE;     // Set the setpoint of the left motor.
        break;
    default:
        break;
    }
}

/* Convert speed in mm/s to a range of 0-100 */
char motor_convert(byte motor, word speed) {
    long converted_speed1, converted_speed2;
    switch(motor) {
    case MOTOR1C:
        converted_speed1 = (((dword)speed * SPEED_RATIO_L) / SPEED_RATIO_DIVISOR) + SPEED_OFFSET_L;
        
        if(converted_speed1 < 0)
            converted_speed1 = 1;
        else if(converted_speed1 > 100)
            converted_speed1 = 100;
        
        return (char)converted_speed1;
        break;
    case MOTOR2C:
        converted_speed2 = (((dword)speed * SPEED_RATIO_R) / SPEED_RATIO_DIVISOR) + SPEED_OFFSET_R;
        
        if(converted_speed2 < 0)
            converted_speed2 = 0;
        else if(converted_speed2 > 100)
            converted_speed2 = 100;
        
        return (char)converted_speed2;
        break;
    default:
        break;
    }
}

// Return absolute value
int abs(int num) {
    if (num < 0)
        return (num * -1);
    else
        return num;
}

/*****************************************************************************/

/* Motor control law interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_MOTOR)) void Control_Law_ISR(void) {
    // Speed and integral error must be a signed value
    static long integral_error1 = 0, integral_error2 = 0;
    long speed_error1 = 0, speed_error2 = 0, read_period1 = 0, read_period2 = 0;
    char drive_value1 = 0, drive_value2 = 0;
    //word cur_count1, cur_count2;
    
    // Calculate error
    read_period1 = (DRIVE_SCALE_VAL / encoder_period(ENC1)) + BVALUE; 
    read_period1 *= MAXDRIVE - MINDRIVE;
    read_period1 /= MINFREQ - MAXFREQ;
    read_period2 = (DRIVE_SCALE_VAL / encoder_period(ENC2)) + BVALUE;
    read_period2 *= MAXDRIVE - MINDRIVE;
    read_period2 /= MINFREQ - MAXFREQ;
    
    speed_error1 = set_point1 - read_period1;
    speed_error2 = set_point2 - read_period2;
    
    
    // Motor 1
    // Check that error is in a realistic range
    if((speed_error1 < MAX_SPEED_ERROR) && (speed_error1 > -MAX_SPEED_ERROR)) {  
        // Check if integration needs to be performed (not at rails)
        if (!((PWM_DTY(MOTOR1_PWM) <= MINDRIVE) && (speed_error1 < 0)) && !((PWM_DTY(MOTOR1_PWM) >= MAXDRIVE) && (speed_error1 > 0)))
            integral_error1 += speed_error1;
        
        // Perform control law calculation
        drive_value1 = (((speed_error1 * P_GAIN) + (integral_error1 * I_GAIN)) / GAIN_DIV) + MINDRIVE;
        
        // Bind to limits
        if (drive_value1 > MAXDRIVE)
            drive_value1 = MAXDRIVE;
        else if ((drive_value1 < MINDRIVE) && (set_point1 == 0))
            drive_value1 = 0;
        else if (drive_value1 < MINDRIVE)
            drive_value1 = MINDRIVE;
        
        PWM_DTY(MOTOR1_PWM) = drive_value1;    // Set motor 1 duty cycle to calculated drive value
        
        /*
        if(set_point1 > 0) {
            if(count_down > 0) {
                PWM_DTY(MOTOR1_PWM) = drive_value1;    // Set motor 1 duty cycle to calculated drive value
            } else
                PWM_DTY(MOTOR1_PWM) = 0;
        }
        */
    }
    
    // Motor 2
    // Check that error is in a realistic range
    if((speed_error2 < MAX_SPEED_ERROR) && (speed_error2 > -MAX_SPEED_ERROR)) {  
        // Check if integration needs to be performed (not at rails)
        if (!((PWM_DTY(MOTOR2_PWM) <= MINDRIVE) && (speed_error2 < 0)) && !((PWM_DTY(MOTOR2_PWM) >= MAXDRIVE) && (speed_error2 > 0)))
            integral_error2 += speed_error2;
        
        // Perform control law calculation
        drive_value2 = (((speed_error2 * P_GAIN) + (integral_error2 * I_GAIN)) / GAIN_DIV) + MINDRIVE;
        
        // Bind to limits
        if (drive_value2 > MAXDRIVE)
            drive_value2 = MAXDRIVE;
        else if ((drive_value2 < MINDRIVE) && (set_point2 == 0))
            drive_value2 = 0;
        else if (drive_value2 < MINDRIVE)
            drive_value2 = MINDRIVE;
        
        PWM_DTY(MOTOR2_PWM) = drive_value2;    // Set motor 2 duty cycle to calculated drive value
        
        /*
        if(set_point2 > 0) {
            if(count_down > 0) {
                PWM_DTY(MOTOR2_PWM) = drive_value2;    // Set motor 1 duty cycle to calculated drive value
                count_down--;
            } else
                PWM_DTY(MOTOR2_PWM) = 0;
        }
        */
    }
    
    TC(TC_MOTOR) = TCNT + MOTOR_CNTL_LAW_DELTA;   // Acknowledge interrupt and rearm to run again
}
