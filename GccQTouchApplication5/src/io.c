#include "io.h"
#include <avr/io.h>
#include <stddef.h>

void io_init(void)
{
    DDRA = (1<<4) | (1<<5);
    DDRB = (1<<0) | (1<<1) | (1<<2);
    PORTA=0;
    PORTB=0;
}

void io_all_on(void)
{
	PORTA |= (1<<4) | (1<<5);
	PORTB |= (1<<0) | (1<<1) | (1<<2);
}

void io_all_off(void)
{
	PORTA &= ~((1<<4) | (1<<5));
	PORTB &= ~((1<<0) | (1<<1) | (1<<2));
	
}

void io_led(enum led l, int state)
{
    volatile unsigned char* port = NULL; 
    int pin;
    switch(l)
    {
        case LED_ROTOR_1: // D1
            port = &PORTB;
            pin = 0;
            break;
        case LED_ROTOR_2: //d2
            port = &PORTB;
            pin = 1;
            break;
        case LED_ROTOR_3: //d3
            port = &PORTB;
            pin = 2;
            break;
        case LED_ROTOR_4: //d4
            port = &PORTA;
            pin = 5;
            break;
        case LED_TEMP:
            port = &PORTA;
            pin = 4;
            break;
    } 
    if(state && port)
        *port |= (1<<pin);
    else if (port)
        *port &= ~(1<<pin);
}

void io_rotor_step(void)
{
    static int state=0;
    int i;
    for(i=0;i<4;i++)
    {
        io_led( i, (i == state ? 1 : 0)  );
    }
    
	state = (state + 1) % 4;
}

void io_rotor_off(void)
{
	int i;
	for(i=0;i<4;i++)
		io_led(i,0);
}

void io_rotor_set(uint8_t state)
{
    int i;
    for(i=0;i<4;i++)
    {
        io_led( i, (i == state ? 1 : 0)  );
    }

}

void io_output_char(char in)
{
	io_led(LED_ROTOR_1,(in & 1));
	io_led(LED_ROTOR_2,(in & 2));
	io_led(LED_ROTOR_3,(in & 4));
	io_led(LED_ROTOR_4,(in & 8));
	
	io_led(LED_TEMP,in);
}