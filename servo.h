/* Servo motor macros */
#ifndef _SERVO_H
#define _SERVO_H

#include "utils.h"


// Servo properties
#define SERVO_PERIOD        20000   // Period in microseconds
#define SERVO_MIN_ON_TIME   600     // Minimum pulse length in microseconds
#define SERVO_MAX_ON_TIME   2400    // Maximum pulse length in microseconds
#define SERVO_INIT_ON_TIME  1500    // Initial pulse length (mid point)

#define SERVO_MIN_ANGLE     0       // Minimum angle in degrees
#define SERVO_MAX_ANGLE     180     // Maximum angle in degrees
#define SERVO_LIMIT_ANGLE   120     // Maximum physical angle in degrees (not used in calculations)

#define SERVO_GAIN          (((SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) * 100) / (SERVO_MAX_ON_TIME - SERVO_MIN_ON_TIME))


#define TC_SERVO            2       // Timer channel used to control pulse to servo

#define SERVO_PORT          PTT
#define SERVO_PORT_DDR      DDRT
#define SERVO_SIG           PTT_PTT2

/*****************************************************************************/

void servo_init(void);
void servo_set_angle(byte);
static void servo_set_times(word, word);


#endif // _SERVO_H