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
#define P_GAIN_DEF      1500
#define I_GAIN_DEF      1500

#define MAXDRIVE        100     // Maximum overall drive value
#define DRIVE_SCALE_VAL 1000000 // Motor drive value scaling
#define GAIN_DIV        1000    // Gain divisor

// Compile time option to choose between 12V and 24V supply
#define TWELVEVOLT
//#define TWENTYFOURVOLT    // ALL DEFINES ARE NOT SETUP YET FOR 24V!

#ifdef  TWELVEVOLT

#define MINDRIVE    0       // Minimum drive value
#define MINPERIOD   845     // Minimum encoder period in microseconds
#define MAXPERIOD   65535   // Maximum encoder period in microseconds
#define MAXSPEED    337     // Maximum speed of left motor in mm/s
#define MINSPEED    0       // Minimum speed of left motor in mm/s

#elif   TWENTYFOURVOLT

#define MINDRIVE    0       // Minimum drive value to overcome detent torque from a stopped position
#define MINPERIOD   370     // Minimum encoder period in microseconds
#define MAXPERIOD   800     // Maximum encoder period in microseconds

#else
#error "Either TWELVEVOLT or TWENTYFOURVOLT must be defined"
#endif

#define MAXFREQ     (DRIVE_SCALE_VAL / MINPERIOD)   // Maximum encoder frequency in kHz
#define MINFREQ     (DRIVE_SCALE_VAL / MAXPERIOD)   // Minimum encoder frequency in kHz
#define BVALUE      -(((DRIVE_SCALE_VAL * (MINDRIVE - MAXDRIVE)) / (MINFREQ - MAXFREQ)) / MAXPERIOD)

#define MAX_SPEED_ERROR     MAXDRIVE    // Limits for error that remain in the range of reality 
#define SPEED_SET_SCALE     100         // Used to adjust speed setpoint

#define SPEED_RATIO         297//(((MAXDRIVE - MINDRIVE) / (MAXSPEED - MINSPEED)) * 1000)
#define SPEED_RATIO_DIVISOR 1000
#define SPEED_OFFSET        0

/*****************************************************************************/

void motor_init(void);
void motor_set_direction(byte, byte);
void motor_set_speed(byte, char);
char motor_convert(byte, int);

static void motor_set_period(byte, byte);
static void motor_set_duty(byte, byte);

int abs(int);

extern long speed_error1, speed_error2, read_period1, read_period2, intermediate_drive_value1, intermediate_drive_value2;
extern byte drive_value1, drive_value2;


#endif // _MOTORS_H