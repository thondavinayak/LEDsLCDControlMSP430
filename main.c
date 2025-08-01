#include <msp430.h>
#include <stdio.h>
#include "time.h"
#include "digital.h"
#include "system.h"
#include "drawings.h"
#include "display.h"

#define BUTTON_UP (P4IN & BIT0)   // P4.0 (UP, red LED)
#define BUTTON_DOWN (P3IN & BIT7)   // P3.7 (DOWN, green LED)

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    int8_t duty_red = 0;
    int8_t duty_green = 0;
    int8_t duty_blue = 0;
    int8_t duty_red_prev = -1;
    int8_t duty_green_prev = -1;
    int8_t duty_blue_prev = -1;
    char buffer[5];

    Init_clock();               // Initialize clock (SMCLK = 8MHz)
    Init_buttons();
    Init_buttons_interrupt();
    init_booster_buttons();
    Init_display();
    Config_pins();
    init_pwm_red();
    init_pwm_green();
    init_pwm_blue();

    __enable_interrupt();

    write_string("Exer 16 ", 80, 20, GRAPHICS_COLOR_BLUE);

    while (1) {
        // Polling for P4.0 (red) and P3.7 (green)
        if (!(BUTTON_UP)) { // P4.0 pressed
            write_string(" RED+ ", 40, 40, GRAPHICS_COLOR_RED);
            duty_red += 5;
            __delay_cycles(200000); // Debounce
        } else if (!(BUTTON_DOWN)) { // P3.7 pressed
            write_string(" GREEN+ ", 40, 40, GRAPHICS_COLOR_GREEN);
            duty_green += 5;
            __delay_cycles(200000); // Debounce
        } else if (get_and_clear_FLAG_L()) { // P2.1 (S1, blue)
            write_string(" BLUE+ ", 40, 40, GRAPHICS_COLOR_BLUE);
            duty_blue += 5;
        } else if (get_and_clear_FLAG_R()) { // P1.1 (S2, reset)
            write_string(" RESET ", 40, 40, GRAPHICS_COLOR_BLUE);
            duty_red = 0;
            duty_green = 0;
            duty_blue = 0;
            pwm_red(duty_red);
            pwm_green(duty_green);
            pwm_blue(duty_blue);
        } else {
            write_string("        ", 40, 40, GRAPHICS_COLOR_BLUE);
        }

        // Update red duty cycle display
        if (duty_red_prev != duty_red) {
            write_string("       ", 100, 40, GRAPHICS_COLOR_RED);
            sprintf(buffer, "%d", duty_red);
            write_string((uint8_t*)buffer, 100, 40, GRAPHICS_COLOR_RED);
            if (pwm_red(duty_red)) {
                rectangle();
            } else {
                clear_rectangle();
            }
            duty_red_prev = duty_red;
        }

        // Update green duty cycle display
        if (duty_green_prev != duty_green) {
            write_string("       ", 100, 60, GRAPHICS_COLOR_GREEN);
            sprintf(buffer, "%d", duty_green);
            write_string((uint8_t*)buffer, 100, 60, GRAPHICS_COLOR_GREEN);
            if (pwm_green(duty_green)) {
                rectangle();
            } else {
                clear_rectangle();
            }
            duty_green_prev = duty_green;
        }

        // Update blue duty cycle display
        if (duty_blue_prev != duty_blue) {
            write_string("       ", 100, 80, GRAPHICS_COLOR_BLUE);
            sprintf(buffer, "%d", duty_blue);
            write_string((uint8_t*)buffer, 100, 80, GRAPHICS_COLOR_BLUE);
            if (pwm_blue(duty_blue)) {
                rectangle();
            } else {
                clear_rectangle();
            }
            duty_blue_prev = duty_blue;
        }
    }
}
