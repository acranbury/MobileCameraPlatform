/* Motor macros */
#ifndef _MOTORS_H
#define _MOTORS_H

#include "utils.h"


// Motor direction ports
#define MOTOR_PORT          PORTB
#define MOTOR_PORT_DDR      DDRB

#define MOTOR1_DIRA         PORTB_BIT0
#define MOTOR1_DIRB         PORTB_BIT1
#define MOTOR2_DIRA         PORTB_BIT2
#define MOTOR2_DIRB         PORTB_BIT3
#define MOTOR1_DIRA_MASK    PORTB_BIT0_MASK
#define MOTOR1_DIRB_MASK    PORTB_BIT1_MASK
#define MOTOR2_DIRA_MASK    PORTB_BIT2_MASK
#define MOTOR2_DIRB_MASK    PORTB_BIT3_MASK

#define MOTOR_BITS          ( MOTOR1_DIRA_MASK | MOTOR1_DIRB_MASK | MOTOR2_DIRA_MASK | MOTOR2_DIRB_MASK )
#define MOTOR1_BITS         ( MOTOR1_DIRA_MASK | MOTOR1_DIRB_MASK )
#define MOTOR2_BITS         ( MOTOR2_DIRA_MASK | MOTOR2_DIRB_MASK )


// Motor numbers
#define MOTOR1          1
#define MOTOR2          2

// Motor number chars
#define MOTOR1C        '0'
#define MOTOR2C        '1'

// Motor PWM channels
#define MOTOR1_PWM      4
#define MOTOR2_PWM      5

// Motor directions (must be binary opposites!)
#define MOTOR_FW        0
#define MOTOR_RV        1

// Motor Control Law Timer channel
#define TC_MOTOR        5

// Motor default period
#define MOTOR_PER_DEF   100   // 0 - 100%.

// Motor default control law delta.
#define MOTOR_CNTL_LAW_DELTA    30000       // Time in micro seconds between each run of the control law.


/* Control Law defines*/

// Initial gain values
#define P_GAIN_DEF  1000
#define I_GAIN_DEF  1000

#define MAXDRIVE    100         // Maximum overall drive value
#define DRIVE_SCALE_VAL 1000000 // Motor drive value scaling
#define GAIN_DIV    1000        // Gain divisor

// Compile time option to choose between 24V and 12V supply. 
#define TWELVEVOLT
//#define TWENTYFOURVOLT

#ifdef  TWELVEVOLT   

#define MINDRIVE   55       // Minimum drive value to overcome detent torque from a stopped position
#define MINPERIOD  800      // Minimum PWM period in microseconds
#define MAXPERIOD  1800     // Maximum PWM period in microseconds
#define MINFREQ    1250
#define MAXFREQ    555
#define BVALUE     293

#elif   TWENTYFOURVOLT

#define MINDRIVE   40       // Minimum drive value to overcome detent torque from a stopped position
#define MINPERIOD  370      // Minimum PWM period in microseconds
#define MAXPERIOD  800      // Maximum PWM period in microseconds
#define MINFREQ    2702
#define MAXFREQ    1250
#define BVALUE     -282

#endif

// Limits for error that remain in the range of reality.
#define MAX_SPEED_ERROR   MAXDRIVE 
#define SPEED_SET_SCALE   100       // Used to adjust speed setpoint.

/*****************************************************************************/

void motor_init(void);
void motor_set_direction(byte, byte);
void motor_set_speed(byte motor, char speed);

static void motor_set_period(byte, byte);
static void motor_set_duty(byte, byte);

int abs(int);

#endif // _MOTORS_H