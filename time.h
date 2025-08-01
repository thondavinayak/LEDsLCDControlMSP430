#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

void Init_timer_no_interrupts(void);
void Timer_Delay_25ms(void);
void Run_timer_interrupt_25ms(void);

extern uint8_t toggle_flag;

uint8_t check_and_clear_toggle_flag();

void init_pwm_red();
uint8_t pwm_red(int8_t duty);
void init_pwm_green();
uint8_t pwm_green(int8_t duty);
void init_pwm_blue();
uint8_t pwm_blue(int8_t duty);

#endif /* TIME_H_ */
