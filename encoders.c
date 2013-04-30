/* Encoders */

#include <hidef.h>      // required for EnableInterrupts and DisableInterrupts macros
#include <mc9s12c32.h>
#include "utils.h"
#include "timer.h"
#include "encoders.h"
#include "motors.h"

#define MAX16BITS   0xFFFF


static word volatile encoder1_count, encoder2_count, encoder1_period, encoder2_period;
static dword volatile dist_setpoint1 = 0, dist_setpoint2 = 0;   // Motor travel distance setpoint in cm


/* Initialize encoders */
void encoder_init(void) {
    
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
    
    encoder1_count = 0;
    encoder2_count = 0;
}

/* Return current encoder count */
word encoder_count(byte encoder) {
    switch(encoder) {
    case ENC1:
        return encoder1_count;
        break;
    case ENC2:
        return encoder2_count;
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
word encoder_period(byte encoder) {
    word period;
    static word prev_overflow = 0;
    word cur_overflow;
    
    cur_overflow = get_overflow_count();
    
    switch(encoder) {
    case ENC1:
        // * Start critical section *
        period = encoder1_period;   // Interrupts not masked off because this function is called within motor control law interrupt
        // * End critical section *
        
        period += (cur_overflow - prev_overflow) * MAX16BITS; 
        break;
    case ENC2:
        // * Start critical section *
        period = encoder2_period;   // Interrupts not masked off because this function is called within motor control law interrupt
        // * End critical section *
        
        period += (cur_overflow - prev_overflow) * MAX16BITS; 
        break;
    default:
        // Incorrect encoder selection, don't update last overflow count
        return 0;
        break;
    }
    
    prev_overflow = cur_overflow;   // Remember count for next time
    
    if(period == 0)
        return 0xFFFF;  // Never return zero (only happens at start when encoder interrupts have never fired)
    
    return period;
}

/* Set motor distance (in cm) */
void motor_set_distance(byte motor, word distance) {
    switch(motor) {
    case MOTOR1C:
        dist_setpoint1 = ((dword)distance * ENCODER_VANE_METRE) / 100;  // Calculate number of encoder vanes to move
        break;
    case MOTOR2C:
        dist_setpoint2 = ((dword)distance * ENCODER_VANE_METRE) / 100;  // Calculate number of encoder vanes to move
        break;
    default:
        break;
    }
}

/*****************************************************************************/

/* Encoder 1 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC1)) void Enc1_ISR(void) {
    static word prev_count = 0;
    word cur_count;
    
    cur_count = TC(TC_ENC1);    // Acknowledge interrupt by accessing timer channel
    
    // Check if set distance has been met
    if(dist_setpoint1 > 0) {
        // Decrement vane distance and check if it has reached zero
        dist_setpoint1--;
        if(dist_setpoint1 == 0)
            motor_set_speed(MOTOR1C, 0);    // Stop motor once distance has been reached
    }
    
    encoder1_period = cur_count - prev_count;
    encoder1_count++;
    
    prev_count = cur_count;     // Remember count for next time
}

/* Encoder 2 interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_ENC2)) void Enc2_ISR(void) {      
    static word prev_count = 0;
    word cur_count;
    
    cur_count = TC(TC_ENC2);    // Acknowledge interrupt by accessing timer channel
    
    // Check if set distance has been met
    if(dist_setpoint2 > 0) {
        // Decrement vane distance and check if it has reached zero
        dist_setpoint2--;
        if(dist_setpoint2 == 0)
            motor_set_speed(MOTOR2C, 0);    // Stop motor once distance has been reached
    }
    
    encoder2_period = cur_count - prev_count;
    encoder2_count++;
    
    prev_count = cur_count;     // Remember count for next time
}