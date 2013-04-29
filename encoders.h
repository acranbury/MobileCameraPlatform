#ifndef _ENCODERS_H
#define _ENCODERS_H

#define ENC1        1
#define ENC2        2

#define TC_ENC1     0
#define TC_ENC2     1

#define ENCODER_VANE_DIST       2741    // Distance of one encoder vane in millimeters times 10000 (actual 0.2741mm per vane)
#define ENCODER_VANE_DIVISOR    10000
//#define ENCODER_VANE_METRE      3648    // Number of encoder vanes in one metre
#define ENCODER_VANE_METRE      3550    // Number of encoder vanes in one metre


/*****************************************************************************/

void encoder_init(void);
word encoder_count(byte encoder);
word encoder_period(byte encoder);
void motor_set_distance(byte, word);


#endif // _ENCODERS_H