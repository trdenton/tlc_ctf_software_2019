#ifndef __IO_H__
#define __IO_H__

#include <avr/io.h>

enum led
{
    LED_ROTOR_1=0,
    LED_ROTOR_2=1,
    LED_ROTOR_3=2,
    LED_ROTOR_4=3,
    LED_TEMP=4
};

void io_init(void);
void io_led(enum led, int);

void io_rotor_step(void);

void io_rotor_set(uint8_t state);

void io_rotor_off(void);
void io_all_on(void);
void io_all_off(void);
void io_output_char(char);
#endif
