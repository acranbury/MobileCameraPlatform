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

long speed_error1 = 0, speed_error2 = 0, read_period1 = 0, read_period2 = 0, intermediate_drive_value1, intermediate_drive_value2;
byte drive_value1 = 0, drive_value2 = 0;


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
char motor_convert(byte motor, int speed) {
    long converted_speed;
    switch(motor) {
    case MOTOR1C:
    case MOTOR2C:
        // Not sure why the return val from abs() must be done first, but it does or it will break!
        converted_speed = abs(speed);
        converted_speed = ((converted_speed * SPEED_RATIO) / SPEED_RATIO_DIVISOR) + SPEED_OFFSET;
        
        if(converted_speed < 0)
            converted_speed = 0;
        else if(converted_speed > 100)
            converted_speed = 100;
        
        return (char)converted_speed;
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
    //long speed_error1 = 0, speed_error2 = 0, read_period1 = 0, read_period2 = 0, intermediate_drive_value1, intermediate_drive_value2;
    //byte drive_value1 = 0, drive_value2 = 0;
    long encoder1_period = 0, encoder2_period = 0;
    static word prev_encoder1_count = 0, prev_encoder2_count = 0;
    word cur_encoder1_count, cur_encoder2_count;
    
    cur_encoder1_count = encoder_count(ENC1);
    cur_encoder2_count = encoder_count(ENC2);
    
    if(cur_encoder1_count == prev_encoder1_count)
        encoder1_period = 0;
    else
        encoder1_period = encoder_period(ENC1);
    
    if(cur_encoder2_count == prev_encoder2_count)
        encoder2_period = 0;
    else
        encoder2_period = encoder_period(ENC2);
    
    
    // Calculate error
    read_period1 = (DRIVE_SCALE_VAL / encoder1_period);
    read_period1 *= (MINDRIVE - MAXDRIVE);
    read_period1 /= (MINFREQ - MAXFREQ);
    read_period1 += BVALUE;
    
    read_period2 = (DRIVE_SCALE_VAL / encoder2_period);
    read_period2 *= (MINDRIVE - MAXDRIVE);
    read_period2 /= (MINFREQ - MAXFREQ);
    read_period2 += BVALUE;
    
    speed_error1 = set_point1 - read_period1;
    speed_error2 = set_point2 - read_period2;
    
    
    // Motor 1
    // Check that error is in a realistic range
    if((speed_error1 >= -MAX_SPEED_ERROR) && (speed_error1 <= MAX_SPEED_ERROR)) {
        // Check if integration needs to be performed (not at rails)
        if (!((PWM_DTY(MOTOR1_PWM) <= MINDRIVE) && (speed_error1 < 0)) && !((PWM_DTY(MOTOR1_PWM) >= MAXDRIVE) && (speed_error1 > 0)))
            integral_error1 += speed_error1;
        
        // Perform control law calculation
        intermediate_drive_value1 = ((((speed_error1 * P_GAIN) + (integral_error1 * I_GAIN)) / GAIN_DIV) + MINDRIVE);
        
        // Bind to limits
        if(intermediate_drive_value1 < MINDRIVE)
            drive_value1 = MINDRIVE;
        else if(intermediate_drive_value1 > MAXDRIVE)
            drive_value1 = MAXDRIVE;
        else
            drive_value1 = (byte)intermediate_drive_value1;
        
        PWM_DTY(MOTOR1_PWM) = drive_value1;    // Set motor 1 duty cycle to calculated drive value
    }
    
    // Motor 2
    // Check that error is in a realistic range
    if((speed_error2 >= -MAX_SPEED_ERROR) && (speed_error2 <= MAX_SPEED_ERROR)) {
        // Check if integration needs to be performed (not at rails)
        if (!((PWM_DTY(MOTOR2_PWM) <= MINDRIVE) && (speed_error2 < 0)) && !((PWM_DTY(MOTOR2_PWM) >= MAXDRIVE) && (speed_error2 > 0)))
            integral_error2 += speed_error2;
        
        // Perform control law calculation
        intermediate_drive_value2 = ((((speed_error2 * P_GAIN) + (integral_error2 * I_GAIN)) / GAIN_DIV) + MINDRIVE);
        
        // Bind to limits
        if(intermediate_drive_value2 < MINDRIVE)
            drive_value2 = MINDRIVE;
        else if(intermediate_drive_value2 > MAXDRIVE)
            drive_value2 = MAXDRIVE;
        else
            drive_value2 = (byte)intermediate_drive_value2;
        
        PWM_DTY(MOTOR2_PWM) = drive_value2;    // Set motor 2 duty cycle to calculated drive value
    }
    
    
    prev_encoder1_count = cur_encoder1_count;
    prev_encoder2_count = cur_encoder2_count;
    
    
    TC(TC_MOTOR) = TCNT + MOTOR_CNTL_LAW_DELTA;   // Acknowledge interrupt and rearm to run again
}
