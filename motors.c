/* Motor functions */


#include <mc9s12c32.h>
#include "utils.h"
#include "pwm.h"
#include "timer.h"
#include "motors.h"


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
    PWM_PER(MOTOR1_PWM) = 0;
    PWM_DTY(MOTOR1_PWM) = 0;
    PWM_PER(MOTOR2_PWM) = 0;
    PWM_DTY(MOTOR2_PWM) = 0;
    
    SET_BITS(MOTOR_PORT_DDR,MOTOR_BITS);    // Set motor lines as outputs
    MOTOR1_DIRA = MOTOR_FW;
    MOTOR1_DIRB = (MOTOR1_DIRA == MOTOR_FW) ? MOTOR_RV : MOTOR_FW;
    //motor_set_direction(MOTOR1, MOTOR_FW);  // Set initial motor direction    <-- can't use motor_set_direction for initial direction setting
    
    // Enable both PWM channels
    PWM_ENABLE(MOTOR1_PWM);
    PWM_ENABLE(MOTOR2_PWM);
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
void motor_set_speed(byte motor, byte speed) {
    switch(motor) {
    case MOTOR1:
        
        break;
    case MOTOR2:
        
        break;
    default:
        break;
    }
}

/*****************************************************************************/

interrupt ?? void control_law_ISR(void) {
    
}
