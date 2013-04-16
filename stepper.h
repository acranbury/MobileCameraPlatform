/* Stepper motor macros */
#ifndef _STEPPER_H
#define _STEPPER_H

#include "utils.h"


#define STEPPER_MAX_ANGLE       180             // Maximum angle in degrees (fully right)
#define STEPPER_DELAY_INIT      5               // Initial step delay in ms
#define STEPPER_STEP_INIT       HALF_STEP_LEFT  // Initial step type
#define TC_STEPPER              4               // Timer channel used to control all four stepper coil lines


#define STEPPER_PORT            PTT
#define STEPPER_PORT_DDR        DDRT

// Stepper coil ports
#define STEPPER_COIL1A          PTT_PTT4
#define STEPPER_COIL1B          PTT_PTT5
#define STEPPER_COIL2A          PTT_PTT6
#define STEPPER_COIL2B          PTT_PTT7
#define STEPPER_COIL1A_MASK     PTT_PTT4_MASK
#define STEPPER_COIL1B_MASK     PTT_PTT5_MASK
#define STEPPER_COIL2A_MASK     PTT_PTT6_MASK
#define STEPPER_COIL2B_MASK     PTT_PTT7_MASK
#define STEPPER_COIL_BITS       ( STEPPER_COIL1A_MASK | STEPPER_COIL1B_MASK | STEPPER_COIL2A_MASK | STEPPER_COIL2B_MASK )

// Stepper limit switch ports
#define STEPPER_LIMIT_PORT      PORTAD0
#define STEPPER_LIMIT_LEFT      PORTAD0_PTAD6
#define STEPPER_LIMIT_RIGHT     PORTAD0_PTAD7
#define STEPPER_LIMIT_INPUT_EN          ATDDIEN
#define STEPPER_LIMIT_INPUT_EN_MASK     ( ATDDIEN_IEN6_MASK | ATDDIEN_IEN7_MASK )


#define HALF_STEP_LEFT      -1
#define HALF_STEP_RIGHT     +1
#define FULL_STEP_LEFT      -2
#define FULL_STEP_RIGHT     +2
#define STEPPER_COUNT_MASK  0x07

/*****************************************************************************/

void stepper_init(void);
void stepper_calibrate(void);
void stepper_reverse(void);
void stepper_set_delay(byte);
void stepper_set_angle(byte);
static void stepper_set_pos(word);

#endif // _STEPPER_H