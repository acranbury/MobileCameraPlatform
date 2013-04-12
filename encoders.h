/* Encoders */

#include <mc9s12c32.h>
#include "utils.h"
#include "timer.h"


#define TC_ENC1     0
#define TC_ENC2     1

/*****************************************************************************/

void encoder_init(void);
word encoder_count(byte);


static word encoder1_count, encoder2_count;

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
    switch(encoder) {
    case 1:
        return encoder1_count;
        break;
    case 2:
        return encoder2_count;
        break;
    default:
        break;
    }
}

/*****************************************************************************/

/* Encoder 1 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC1)) void Enc1_ISR(void) {
    (void)TC(TC_ENC1);  // Acknowledge interrupt by accessing timer channel
    encoder1_count++;
}

/* Encoder 2 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC2)) void Enc2_ISR(void) {
    (void)TC(TC_ENC2);  // Acknowledge interrupt by accessing timer channel
    encoder2_count++;
}