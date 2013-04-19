/* Encoders */

#include <hidef.h>      // required for EnableInterrupts and DisableInterrupts macros
#include <mc9s12c32.h>
#include "utils.h"
#include "timer.h"
#include "encoders.h"

#define MAX16BITS   0xFFFF

static long encoder1_period;
static long encoder2_period;
static word encoder1_count; 
static word encoder2_count;

/* Initialize encoders */
void encoder_init(void) {
    DisableInterrupts;
      encoder1_count = 0;
      encoder2_count = 0;
    EnableInterrupts;
    
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
        DisableInterrupts;
          count = encoder1_count;
        EnableInterrupts;
        return count;
        break;
    case 2:
        DisableInterrupts;
          count = encoder2_count;
        EnableInterrupts;
        return count;
        break;
    default:
        return 0;
        break;
    }
}

/* Return current encoder count */
/* ONLY call from motor control law interrupt*/
long encoder_period(byte encoder) {
    volatile long count;
    switch(encoder) {
    case 1:
        count = encoder1_period;
        count += get_overflow_count() * (MAX16BITS); 
        reset_overflow_count();
        break;
    case 2:
        count = encoder2_period;
        count += get_overflow_count() * (MAX16BITS); 
        reset_overflow_count();
        break;
    default:
        count = 0;
        break;
    }
    return count;
}

/*****************************************************************************/

/* Encoder 1 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC1)) void Enc1_ISR(void) {
    static last_count_1 = 0;
    static cur_count_1 = 0;
    cur_count_1 = TC(TC_ENC1);  // Acknowledge interrupt by accessing timer channel
    encoder1_period = cur_count_1 - last_count_1;
    encoder1_count++;
    last_count_1 = cur_count_1;
}

/* Encoder 2 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC2)) void Enc2_ISR(void) {      
    static last_count_2 = 0;
    static cur_count_2 = 0;
    cur_count_2 = TC(TC_ENC2);  // Acknowledge interrupt by accessing timer channel
    encoder2_period = cur_count_2 - last_count_2;
    encoder2_count++;
    last_count_2 = cur_count_2;
}