/* Encoders */

#include <hidef.h>      // required for EnableInterrupts and DisableInterrupts macros
#include <mc9s12c32.h>
#include "utils.h"
#include "timer.h"
#include "encoders.h"

#define MAX16BITS   0xFFFF


static word volatile encoder1_count;
static word volatile encoder2_count;
static long volatile encoder1_period;
static long volatile encoder2_period;

/* Initialize encoders */
void encoder_init(void) {
    encoder1_count = 0;
    encoder2_count = 0;
    
    // Encoder channels set to input capture
    TC_IC(TC_ENC1);
    TC_IC(TC_ENC2);
    
    // Trigger input capture on falling edge
    // IR LED on the encoder sensor has faster fall-time than rising-time
    SET_IC_EDGE(TC_ENC1,IC_DETECT_FALLING);
    SET_IC_EDGE(TC_ENC2,IC_DETECT_FALLING);
    
    // enable timer channel interrupts
    TC_INT_ENABLE(TC_ENC1);
    TC_INT_ENABLE(TC_ENC2);
}

/* Return current encoder count */
word encoder_count(byte encoder) {
    word count;
    switch(encoder) {
    case 1:
        count = encoder1_count;
        return count;
        break;
    case 2:
        count = encoder2_count;
        return count;
        break;
    default:
        // Incorrect encoder selection
        return 0;
        break;
    }
}

/*             Return current encoder period             */
/*      ONLY call from motor control law interrupt!      */
/* Mutual exclusion is not enforced in critical sections */
dword encoder_period(byte encoder) {
    dword period;
    static word prev_overflow = 0;
    word cur_overflow;
    
    cur_overflow = get_overflow_count();
    
    switch(encoder) {
    case 1:
        // * Critical section *
        period = encoder1_period;   // Interrupts not masked off because this function is called within motor control law interrupt
        
        period += (cur_overflow - prev_overflow) * MAX16BITS; 
        break;
    case 2:
        // * Critical section *
        period = encoder2_period;   // Interrupts not masked off because this function is called within motor control law interrupt
        
        period += (cur_overflow - prev_overflow) * MAX16BITS; 
        break;
    default:
        // Incorrect encoder selection
        period = 0;
        break;
    }
    
    prev_overflow = cur_overflow;   // Remember count for next time
    
    return period;
}

/*****************************************************************************/

/* Encoder 1 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC1)) void Enc1_ISR(void) {
    static word prev_count = 0;
    word cur_count;
    
    cur_count = TC(TC_ENC1);    // Acknowledge interrupt by accessing timer channel
    
    encoder1_period = cur_count - prev_count;
    encoder1_count++;
    
    prev_count = cur_count;     // Remember count for next time
}

/* Encoder 2 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC2)) void Enc2_ISR(void) {      
    static word prev_count = 0;
    word cur_count;
    
    cur_count = TC(TC_ENC2);    // Acknowledge interrupt by accessing timer channel
    
    encoder2_period = cur_count - prev_count;
    encoder2_count++;
    
    prev_count = cur_count;     // Remember count for next time
}