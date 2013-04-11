/* Timer module functions */

#include <mc9s12c32.h>
#include "utils.h"
#include "timer.h"


/* Initialize timer module */
void timer_init(void) {
    timer_overflow_count = 0;   // Reset overflow counter
    
    // Currently the timer overflow interrupt occurs too frequently with a prescaler of 8
    //TOI_ENABLE;                 // Enable TCNT overflow interrupt
    
    SET_TCNT_PRESCALE(TCNT_PRESCALE_8);     // Set timer prescaler
    SET_BITS(TSCR1,TSCR1_INIT);             // Set timer operation modes and enable timer
}

/*      Milisecond sleep timer       */
/* Uses output compare timer channel */
/* to count in milisecond increments */
void msleep(word ms) {
    word i;
    
    // Enable timer module if not already enabled
    if(!(TSCR1 & TSCR1_TEN_MASK)) EnableTimer;
    
    TC(TC_SLEEP) = TCNT + OC_DELTA_1MS; // Preset channel register
    TC_OC(TC_SLEEP);                    // Enable channel as output compare
    
    for(i=0; i < ms; i++) {
        while(!(TFLG1 & (1 << TC_SLEEP)));  // Wait for event
        TC(TC_SLEEP) += OC_DELTA_1MS;       // Rearm channel register, clearing TFLG as well
    }
}

/*****************************************************************************/

/* TCNT overflow interrupt handler */
interrupt VectorNumber_Vtimovf void TCNT_Overflow_ISR(void) {
    TFLG2 = TFLG2_TOF_MASK;     // Clear timer overflow flag by writing a one to it
    timer_overflow_count++;
}
