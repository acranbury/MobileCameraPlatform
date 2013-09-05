/* PWM module functions */


#include <mc9s12c32.h>
#include "utils.h"
#include "pwm.h"


/* Initialize PWM module */
void PWMinit(void) {
    SET_BITS(PWMCTL,( PWMCTL_PFRZ_MASK | PWM_8BIT_ALL ));   // Set PWM mode
    SET_PWM_PRESCALE(PWM_PRESCALE_32,PWM_PRESCALE_32);        // Set prescaler for PWM clocks A and B
    
    // Set scaled clock divisor
    PWM_SA_SCALE(1);    // clock SA = clock A / 2
    PWM_SB_SCALE(1);    // clock SB = clock B / 2
}
