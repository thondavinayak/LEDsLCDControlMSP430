#include "system.h"
#include "display.h"
#include <intrinsics.h>
#include <msp430.h>
#include <stdint.h>
#include <drawings.h>
#include <digital.h>

static volatile uint8_t FLAG_L = 0;
static volatile uint8_t FLAG_R = 0;
static volatile uint8_t state = 0;

void Config_pins(void) {
    P2DIR |= BIT4;           // P2.4 (red LED) as output
    P1DIR |= BIT4 | BIT5;    // P1.4 (blue LED), P1.5 (green LED) as outputs
    P2OUT &= ~BIT4;          // Initialize red to low
    P1OUT &= ~(BIT4 | BIT5); // Initialize blue, green to low

    // Initialize unused ports to reduce power consumption
    P3DIR = 0xFF; P3OUT = 0x00; // Port 3 all outputs, low
    P4DIR = 0xFF; P4OUT = 0x00; // Port 4 all outputs, low
    P5DIR = 0xFF; P5OUT = 0x00; // Port 5 all outputs, low
    P6DIR = 0xFF; P6OUT = 0x00; // Port 6 all outputs, low
    PJDIR = 0xFF; PJOUT = 0x00; // Port J all outputs, low
}

void Init_buttons(void) {
    P2DIR &= ~BIT1;  // P2.1 input button_L (S1, blue)
    P2REN |= BIT1;
    P2OUT |= BIT1;

    P1DIR &= ~BIT1;  // P1.1 input button_R (S2, reset)
    P1REN |= BIT1;
    P1OUT |= BIT1;
}

void Init_buttons_interrupt(void) {
    P2IES |= BIT1;   // Falling edge for P2.1 (S1)
    P2IE |= BIT1;
    P2IFG &= ~BIT1;

    P1IES |= BIT1;   // Falling edge for P1.1 (S2)
    P1IE |= BIT1;
    P1IFG &= ~BIT1;
}

void init_booster_buttons(void) {
    P4DIR &= ~BIT0;   // P4.0 (UP, red) as input
    P4REN |= BIT0;
    P4OUT |= BIT0;

    P3DIR &= ~BIT7;   // P3.7 (DOWN, green) as input
    P3REN |= BIT7;
    P3OUT |= BIT7;
}

void Init_rgb_led(void) {
    P2DIR |= BIT4;           // P2.4 (red)
    P1DIR |= BIT4 | BIT5;    // P1.4 (blue), P1.5 (green)
    P2OUT &= ~BIT4;          // Initialize red to low
    P1OUT &= ~(BIT4 | BIT5); // Initialize blue, green to low
}

void round_robin_led(void) {
    state++;
    state = state % 3;

    switch(state) {
        case 0:
            P1OUT &= ~(BIT4 | BIT5);
            P2OUT |= BIT4;
            write_string("red   ", 4, 100, GRAPHICS_COLOR_BLACK);
            break;
        case 1:
            P1OUT &= ~BIT4;
            P2OUT &= ~BIT4;
            P1OUT |= BIT5;
            write_string("green ", 4, 100, GRAPHICS_COLOR_BLACK);
            break;
        case 2:
            P1OUT &= ~BIT5;
            P2OUT &= ~BIT4;
            P1OUT |= BIT4;
            write_string("blue  ", 4, 100, GRAPHICS_COLOR_BLACK);
            break;
    }
}

void round_robin_reset(void) {
    state = 0;
    write_string("reset ", 4, 100, GRAPHICS_COLOR_BLACK);
    P1OUT &= ~(BIT5 | BIT4);
    P2OUT &= ~BIT4;
}

uint8_t get_and_clear_FLAG_L(void) {
    uint16_t GIE_state = __get_SR_register() & GIE;
    __disable_interrupt();
    uint8_t temp = FLAG_L;
    FLAG_L = 0;
    __bis_SR_register(GIE_state);
    return temp;
}

uint8_t get_and_clear_FLAG_R(void) {
    uint16_t GIE_state = __get_SR_register() & GIE;
    __disable_interrupt();
    uint8_t temp = FLAG_R;
    FLAG_R = 0;
    __bis_SR_register(GIE_state);
    return temp;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void) {
    if (P2IFG & BIT1) {
        FLAG_L = 1;  // S1 (blue LED)
        P2IFG &= ~BIT1;
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    if (P1IFG & BIT1) {
        FLAG_R = 1;  // S2 (reset)
        P1IFG &= ~BIT1;
    }
}
