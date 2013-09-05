/* PWM module macros */
#ifndef _PWM_H
#define _PWM_H

#include "utils.h"


// PWM mode macros
#define PWM_8BIT_ALL        0x00    // All six PWM channels are 8-bit
#define PWM_16BIT_01        PWMCTL_CON01_MASK   // Concatenate channels 0 and 1 into a 16-bit channel
#define PWM_16BIT_23        PWMCTL_CON23_MASK   // Concatenate channels 2 and 3 into a 16-bit channel
#define PWM_16BIT_45        PWMCTL_CON45_MASK   // Concatenate channels 4 and 5 into a 16-bit channel
#define PWM_16BIT_ALL       ( PWMCTL_CON01_MASK | PWMCTL_CON23_MASK | PWMCTL_CON45_MASK )   // Concatenate six 8-bit channels into three 16-bit channels


// PWM clock A and B prescale macros to set one of 8 possible prescale values per clock
#define SET_PWM_PRESCALE(scale_A,scale_B)   FORCE_BITS(PWMPRCLK,(PWMPRCLK_PCKA_MASK | PWMPRCLK_PCKB_MASK),((scale_A) | (scale_B) << PWMPRCLK_PCKB_BITNUM))
#define SET_PWM_PRESCALE_A(scale_A)         FORCE_BITS(PWMPRCLK,PWMPRCLK_PCKA_MASK,(scale_A))
#define SET_PWM_PRESCALE_B(scale_B)         FORCE_BITS(PWMPRCLK,PWMPRCLK_PCKB_MASK,(scale_B) << PWMPRCLK_PCKB_BITNUM)
#define PWM_PRESCALE_1                      0x00
#define PWM_PRESCALE_2                      0x01
#define PWM_PRESCALE_4                      0x02
#define PWM_PRESCALE_8                      0x03
#define PWM_PRESCALE_16                     0x04
#define PWM_PRESCALE_32                     0x05
#define PWM_PRESCALE_64                     0x06
#define PWM_PRESCALE_128                    0x07


// PWM Scaled A and B clock divisor (8 bits)
// eg. clock SA = clock A / (2 * value)
// A value of zero is considered full scale value of 256 (clock divided by 512)
#define PWM_SA_SCALE(value)     (PWMSCLA = (value))
#define PWM_SB_SCALE(value)     (PWMSCLB = (value))


// PWM channel enable/disable
#define PWM_ENABLE(chan)        SET_BITS(PWME, 1 << (chan))     // Enable PWM channel (0-5)
#define PWM_ENABLE_ALL          SET_BITS(PWME,PWME_PWME_MASK)   // Enable all PWM channels at once
#define PWM_DISABLE(chan)       CLR_BITS(PWME, 1 << (chan))     // Disable PWM channel (0-5)
#define PWM_DISABLE_ALL         CLR_BITS(PWME,PWME_PWME_MASK)   // Disable all PWM channels at once


// PWM channel starting polarity
#define PWM_POL(chan,pol)       SET_BITS(PWMPOL,(pol) << (chan))
#define PWM_POL_LO              0x00
#define PWM_POL_HI              0x01

// PWM channel clock source select
// Channels 0-1, 4-5:   clock A or clock SA
// Channels 2-3:        clock B or clock SB
#define PWM_CLK(chan,clk)       SET_BITS(PWMCLK,(clk) << (chan))
#define PWM_CLK_A               0x00
#define PWM_CLK_SA              0x01
#define PWM_CLK_B               0x00
#define PWM_CLK_SB              0x01

// PWM channel left-align/center-align
#define PWM_CENTER_ALIGN(chan)  SET_BITS(PWMCAE, 1 << (chan))
#define PWM_LEFT_ALIGN(chan)    CLR_BITS(PWMCAE, 1 << (chan))

#define PWM_PER(chan)           CAT(PWMPER,chan)
#define PWM_DTY(chan)           CAT(PWMDTY,chan)
#define PWM_CNT(chan)           CAT(PWMCNT,chan)

/*****************************************************************************/

void PWMinit(void);


#endif // _PWM_H