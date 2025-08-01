#include <msp430.h>
#include "time.h"

uint8_t toggle_flag = 0; // Global flag for main

static uint8_t pending_update_red = 0;
static uint16_t new_duty_value_red = 0;
static uint8_t pending_update_green = 0;
static uint16_t new_duty_value_green = 0;
static uint8_t pending_update_blue = 0;
static uint16_t new_duty_value_blue = 0;

void Init_timer_no_interrupts(void) {
    TA1CTL = TASSEL_2 | ID_3 | TACLR; // SMCLK, divide by 8, clear timer, stopped
}

void Run_timer_interrupt_25ms(void) {
    TA1CCR0 = 24999;                  // 25ms at 1MHz (8MHz/8 = 1MHz, 25000 counts)
    TA1CCTL0 = CCIE;                  // Enable capture/compare interrupt for CCIFG
    TA1CTL |= TACLR;                  // Clear timer
    TA1CCTL0 &= ~CCIFG;               // Clear CCIFG flag
    TA1CTL |= MC_1;                   // Up mode
}

uint8_t check_and_clear_toggle_flag() {
    uint16_t gie = __get_SR_register() & GIE;
    __disable_interrupt();
    uint8_t local_flag = toggle_flag;
    toggle_flag = 0;
    __bis_SR_register(gie);
    return local_flag;
}

void init_pwm_red() {
    TA2CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // SMCLK, divide by 8, Up mode, clear timer
    TA2CCR0 = 1249;                          // 1.25ms = 1250 ticks at 1MHz (800Hz)
    TA2CCR1 = 0;                             // 0% duty cycle
    TA2CCTL1 = OUTMOD_7;                     // Reset/Set for PWM
    P2DIR |= BIT4;                           // P2.4 (red LED) as output
    P2SEL |= BIT4;                           // Enable PWM function (TA2.1)
}

uint8_t pwm_red(int8_t duty) {
    if (duty > 100 || duty < 0) return 1;

    if (duty < 5) {
        TA2CCTL1 = 0;     // LED off
        TA2CCTL1 &= ~OUT;
    } else if (duty > 95) {
        TA2CCTL1 = 0;
        TA2CCTL1 |= OUT;  // Force LED on
    } else {
        TA2CCTL1 = OUTMOD_7;
        new_duty_value_red = ((uint32_t)duty * 1249) / 100;
        pending_update_red = 1;
        TA2CCTL1 |= CCIE;
    }
    return 0;
}

void init_pwm_green() {
    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // SMCLK, divide by 8, Up mode, clear timer
    TA0CCR0 = 1249;                          // 1.25ms = 1250 ticks at 1MHz (800Hz)
    TA0CCR1 = 0;                             // 0% duty cycle
    TA0CCTL1 = OUTMOD_7;                     // Reset/Set for PWM
    P1DIR |= BIT5;                           // P1.5 (green LED) as output
    P1SEL |= BIT5;                           // Enable PWM function (TA0.1)
}

uint8_t pwm_green(int8_t duty) {
    if (duty > 100 || duty < 0) return 1;

    if (duty < 5) {
        TA0CCTL1 = 0;     // LED off
        TA0CCTL1 &= ~OUT;
    } else if (duty > 95) {
        TA0CCTL1 = 0;
        TA0CCTL1 |= OUT;  // Force LED on
    } else {
        TA0CCTL1 = OUTMOD_7;
        new_duty_value_green = ((uint32_t)duty * 1249) / 100;
        pending_update_green = 1;
        TA0CCTL1 |= CCIE;
    }
    return 0;
}

void init_pwm_blue() {
    TA2CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // SMCLK, divide by 8, Up mode, clear timer
    TA2CCR0 = 1249;                          // 1.25ms = 1250 ticks at 1MHz (800Hz)
    TA2CCR2 = 0;                             // 0% duty cycle
    TA2CCTL2 = OUTMOD_7;                     // Reset/Set for PWM
    P1DIR |= BIT4;                           // P1.4 (blue LED) as output
    P1SEL |= BIT4;                           // Enable PWM function (TA2.2)
}

uint8_t pwm_blue(int8_t duty) {
    if (duty > 100 || duty < 0) return 1;

    if (duty < 5) {
        TA2CCTL2 = 0;     // LED off
        TA2CCTL2 &= ~OUT;
    } else if (duty > 95) {
        TA2CCTL2 = 0;
        TA2CCTL2 |= OUT;  // Force LED on
    } else {
        TA2CCTL2 = OUTMOD_7;
        new_duty_value_blue = ((uint32_t)duty * 1249) / 100;
        pending_update_blue = 1;
        TA2CCTL2 |= CCIE;
    }
    return 0;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1_CCR0_ISR(void) {
    static uint8_t interrupt_counter = 0;
    interrupt_counter++;
    if (interrupt_counter >= 48) {
        toggle_flag = 1;
        interrupt_counter = 0;
    }
    TA1CCTL0 &= ~CCIFG;
}

#pragma vector=TIMER2_A1_VECTOR
__interrupt void Timer2_A1_ISR(void) {
    if (TA2CCTL1 & CCIFG) {
        TA2CCTL1 &= ~CCIFG;
        if (pending_update_red) {
            TA2CCR1 = new_duty_value_red;
            pending_update_red = 0;
            TA2CCTL1 &= ~CCIE;
        }
    }
    if (TA2CCTL2 & CCIFG) {
        TA2CCTL2 &= ~CCIFG;
        if (pending_update_blue) {
            TA2CCR2 = new_duty_value_blue;
            pending_update_blue = 0;
            TA2CCTL2 &= ~CCIE;
        }
    }
    if (TA2CTL & TAIFG) {
        TA2CTL &= ~TAIFG;
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void) {
    if (TA0CCTL1 & CCIFG) {
        TA0CCTL1 &= ~CCIFG;
        if (pending_update_green) {
            TA0CCR1 = new_duty_value_green;
            pending_update_green = 0;
            TA0CCTL1 &= ~CCIE;
        }
    }
    if (TA0CTL & TAIFG) {
        TA0CTL &= ~TAIFG;
    }
}
