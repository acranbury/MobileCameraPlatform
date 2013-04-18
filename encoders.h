#ifndef _ENCODERS

#define _ENCODERS 
#define TC_ENC1     0
#define TC_ENC2     1

/*****************************************************************************/

void encoder_init(void);
word encoder_count(byte encoder);
long encoder_period(byte encoder);

#endif