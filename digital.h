#ifndef DIGITAL_H_
#define DIGITAL_H_

#include <stdint.h>
#include <msp430.h>

void Init_buttons(void);
void Init_buttons_interrupt(void);
uint8_t get_and_clear_FLAG_L(void);
uint8_t get_and_clear_FLAG_R(void);
void Init_rgb_led(void);
void init_booster_buttons(void);
void init_booster_buttons_interrupt(void);
void round_robin_led(void);
void round_robin_reset(void);
void Config_pins(void);

#endif /* DIGITAL_H_ */
