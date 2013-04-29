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


// Motor PWM channels
#define MOTOR1_PWM      4
#define MOTOR2_PWM      5

// Motor Control Law Timer channel
#define TC_MOTOR        5


// Motor numbers
#define MOTOR1          1
#define MOTOR2          2

// Motor numbers in ASCII
#define MOTOR1C        '0'
#define MOTOR2C        '1'

// Motor directions (must be binary opposites!)
// These depend on the orientation of the motor header plugs
#define MOTOR_FW        0
#define MOTOR_RV        1

// Motor default period
#define MOTOR_PER_DEF   100     // 0 - 100%

// Motor default control law delta
#define MOTOR_CNTL_LAW_DELTA    30000   // Time in microseconds between each run of the control law


/* Control Law defines */

// Initial gain values
#define P_GAIN_DEF      1000
#define I_GAIN_DEF      1000

#define MAXDRIVE        100     // Maximum overall drive value
#define DRIVE_SCALE_VAL 1000000 // Motor drive value scaling
#define GAIN_DIV        1000    // Gain divisor

// Compile time option to choose between 12V and 24V supply
#define TWELVEVOLT
//#define TWENTYFOURVOLT

#ifdef  TWELVEVOLT

#define MINDRIVE    55       // Minimum drive value to overcome detent torque from a stopped position
#define MINPERIOD   800      // Minimum PWM period in microseconds
#define MAXPERIOD   1800     // Maximum PWM period in microseconds
#define MINFREQ     1250     // Minimum PWM frequency in microhertz
#define MAXFREQ     555      // Maximum PWM frequency in microhertz
#define BVALUE      293
#define MAXSPEED_L  337     // Maximum speed of left motor in mm/s
#define MINSPEED_L  204     // Minimum speed of left motor in mm/s
#define MAXSPEED_R  331
#define MINSPEED_R  196

#elif   TWENTYFOURVOLT

#define MINDRIVE    40       // Minimum drive value to overcome detent torque from a stopped position
#define MINPERIOD   370      // Minimum PWM period in microseconds
#define MAXPERIOD   800      // Maximum PWM period in microseconds
#define MINFREQ     2702     // Minimum PWM frequency in microhertz
#define MAXFREQ     1250     // Maximum PWM frequency in microhertz
#define BVALUE      -282

#else
#error "Either TWELVEVOLT or TWENTYFOURVOLT must be defined"
#endif

#define MAX_SPEED_ERROR     MAXDRIVE    // Limits for error that remain in the range of reality 
#define SPEED_SET_SCALE     100         // Used to adjust speed setpoint
#define SPEED_RATIO_L       744 //(((100 - 1) / (MAXSPEED_L - MINSPEED_L)) * 1000)    // 0.74*1000
#define SPEED_RATIO_R       733 //(((100 - 1) / (MAXSPEED_R - MINSPEED_R)) * 1000)    // 0.73*1000
#define SPEED_RATIO_DIVISOR 1000
#define SPEED_OFFSET_L      -150    // 100 - SPEED_RATIO_L(MAXSPEED_L)
#define SPEED_OFFSET_R      -143    // 100 - SPEED_RATIO_R(MAXSPEED_R)

/*****************************************************************************/

void motor_init(void);
void motor_set_direction(byte, byte);
void motor_set_speed(byte, char);
char motor_convert(byte, word);

static void motor_set_period(byte, byte);
static void motor_set_duty(byte, byte);

int abs(int);


#endif // _MOTORS_H