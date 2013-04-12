/* PWM module functions */


#include <mc9s12c32.h>
#include "utils.h"

#define MOTOR1_PWM      4
#define MOTOR2_PWM      5


/* Initialize PWM module */
void PWMinit(void) {
    SET_BITS(PWMCTL,( PWMCTL_PFRZ_MASK | PWM_8BIT_ALL ));   // Set PWM mode
    SET_PWM_PRESCALE(PWM_PRESCALE_1,PWM_PRESCALE_1);        // Set prescaler for PWM clocks A and B
    
    // Set scaled clock divisor
    PWM_SA_SCALE(1);    // clock SA = clock A / 2
    PWM_SB_SCALE(1);    // clock SB = clock B / 2
}

/* Initialize motor PWM channels */
void motor_init(void) {
    
    // Set prescaler for source clock
    SET_PWM_PRESCALE_A(PWM_PRESCALE_8);
    
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
    PWM_PER(MOTOR1_PWM) = ;
    PWM_DTY(MOTOR1_PWM) = ;
    
    PWM_PER(MOTOR2_PWM) = ;
    PWM_DTY(MOTOR2_PWM) = ;
    
    // Enable both PWM channels
    PWM_ENABLE(MOTOR1_PWM);
    PWM_ENABLE(MOTOR2_PWM);
}

/* Set motor PWM period */
void motor_set_period(byte motor, byte period) {
    switch(motor) {
    case 1:
        PWM_PER(MOTOR1_PWM) = period;
        break;
    case 2:
        PWM_PER(MOTOR2_PWM) = period;
        break;
    default:
        break;
    }
}

/* Set motor PWM duty cycle */
void motor_set_duty(byte motor, byte duty) {
    switch(motor) {
    case 1:
        PWM_DTY(MOTOR1_PWM) = duty;
        break;
    case 2:
        PWM_DTY(MOTOR2_PWM) = duty;
        break;
    default:
        break;
    }
}
