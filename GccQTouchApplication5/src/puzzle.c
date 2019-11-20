#include <stdbool.h>
#include <string.h>
#define F_CPU 8000000UL
#include <util/delay.h>

static int last_observed_rotor=0;
static int rotor_count=0;
void puzzle_rotor_reset(void)
{
	
	last_observed_rotor=0;
	rotor_count=0;
}
//we're going to get a series [0,1,2,3]
//need to check if its gone through rotations 8 times
bool puzzle_rotor_step(int sensed)
{
    if(sensed == (last_observed_rotor+1)%4)
        rotor_count++; 
    else if (sensed != last_observed_rotor)
        rotor_count=0;
   
    last_observed_rotor = sensed; 

    if (rotor_count >= 4*4)
        return true;
    return false;
}
bool puzzle_temp_step(int temp)
{
    static int count=0;
    if(temp <= 10)
        count++;
    else
        count=0;
    if(count >= 4)
        return true;
    return false;
}


//led_cb accepts a byte to output
//delay_ms calls a delay loop
//out is the string to display
static void pattern_output(void(*led_cb)(char),void(*delay_ms_cb)(unsigned long),const char* out)
{
    int i;
    for(i=0;i<strlen(out);i++)
    {
        led_cb( (out[i]&0xf0) >>4 );
        _delay_ms(1000);
        led_cb(0);
        _delay_ms(500);
		led_cb(out[i]&0x0f);
		_delay_ms(1000);
		led_cb(0);
		_delay_ms(500);
    }
}

void puzzle_rotor_solved(void(*led_cb)(char),void(*delay_ms_cb)(unsigned long))
{
    char *out = "QMIT";
    pattern_output(led_cb,delay_ms_cb,out);

}

void puzzle_temp_solved(void(*led_cb)(char),void(*delay_ms_cb)(unsigned long))
{
    char *out = "L6MV";
    pattern_output(led_cb,delay_ms_cb,out);
}
