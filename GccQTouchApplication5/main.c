/*******************************************************************************
*   $FILE:  main.c
*   Atmel Corporation:  http://www.atmel.com \n
*   Support email:  www.atmel.com/design-support/
******************************************************************************/

/*  License
*   Copyright (c) 2010, Atmel Corporation All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
*
*   1. Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*
*   2. Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
*   3. The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
*   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
*   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*----------------------------------------------------------------------------
                            compiler information
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                include files
----------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <time.h>
#define __delay_cycles(n)     __builtin_avr_delay_cycles(n)
#define __enable_interrupt()  sei()

#include "touch_api.h"
#include "touch.h"

#include "src/io.h"
#include "src/adc.h"
#include "src/puzzle.h"
/*----------------------------------------------------------------------------
                            manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                    macros
----------------------------------------------------------------------------*/

#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))
#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]

/*----------------------------------------------------------------------------
                            type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                prototypes
----------------------------------------------------------------------------*/
extern void touch_measure(void);
extern void touch_init( void );
extern void init_system( void );
extern void init_timer_isr(void);
extern void set_timer_period(uint16_t);
/*----------------------------------------------------------------------------
                            Structure Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                    macros
----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
                                global variables
----------------------------------------------------------------------------*/
/* Timer period in msec. */
uint16_t qt_measurement_period_msec = QT_MEASUREMENT_PERIOD_MS;
uint16_t time_ms_inc=0;
/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                static variables
----------------------------------------------------------------------------*/

/* flag set by timer ISR when it's time to measure touch */
volatile uint8_t time_to_measure_touch = 0u;

/* current time, set by timer ISR */
volatile uint16_t current_time_ms_touch = 0u;

volatile uint64_t current_time = 0u;
uint64_t last_poke_time = 0u;
uint64_t last_temp_time = 0u;

uint8_t get_rotor_pos()
{
	uint8_t pos = qt_measure_data.qt_touch_status.rotor_slider_values[0];
	pos >>= 6;//top 2 bits give us index of led to light up
	pos &= 0x03;
	return pos;
}

bool rotor_touched()
{
	if(qt_measure_data.qt_touch_status.sensor_states[0])
		return true;
	return false;
}

void update_rotor_leds(void)
{
	if(rotor_touched())
	{
		uint8_t pos = get_rotor_pos();
		io_rotor_set(pos);
	}
	else
		io_rotor_off();
}
void song_and_dance(void)
{
		int d = 250;
		//do a startup pattern
		int i;
		for(i=0;i<4;i++)
		{
			io_all_on();
			_delay_ms(d);
			io_all_off();
			_delay_ms(d);
		}
		
		io_all_off();
		for(i=0;i<2;i++)
		{
			io_led(LED_TEMP,0);
			_delay_ms(100);
			io_led(LED_TEMP,1);
			_delay_ms(100);
		}
}
/*============================================================================
Name    :   main
------------------------------------------------------------------------------
Purpose :   main code entry point
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/

static void delay_cb(unsigned long in)
{
	_delay_ms(1000);
}
int main( void )
{
	uint16_t temperature;
   /* initialise host app, pins, watchdog, etc */
    init_system();

    /* configure timer ISR to fire regularly */
    init_timer_isr();

	/* Initialize Touch sensors */
	touch_init();

	io_init();
	adc_init();
    /* enable interrupts */
    __enable_interrupt();
	
	song_and_dance();
	temperature = adc_get_reading();
	io_all_off();
    /* loop forever */
	bool rotor_solved = false;
    for( ; ; )
    {
        touch_measure();
		update_rotor_leds();
		
		if (rotor_touched())
		{
			uint8_t step = get_rotor_pos();
			last_poke_time = current_time;
			if(puzzle_rotor_step(step))
			{
				rotor_solved=true;
			}
		}
		if (current_time - last_poke_time > 1000)
			puzzle_rotor_reset();
		
		if(rotor_solved)
		{
			puzzle_rotor_reset();
			rotor_solved=false;
			
			puzzle_rotor_solved(io_output_char,delay_cb);
		}
		
		uint16_t t = adc_get_reading();
		
		//if they drop the temperature by 15 degrees, output
		
		if (t <= (temperature - 15))
		{
			if (current_time - last_temp_time > 30000UL)//30 seconds
			{
				puzzle_temp_solved(io_output_char,delay_cb);
				last_temp_time =current_time;			
			}
		}
			
		
		
    /*  Time Non-critical host application code goes here  */
    }
}

