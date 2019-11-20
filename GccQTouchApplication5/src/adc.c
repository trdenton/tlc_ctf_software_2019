#include <avr/io.h>

void adc_init()
{

	/*
	   Selecting the ADC8 channel by writing the MUX[5:0] bits in
	   ADMUX register to “100010” enables the temperature sensor. The internal 1.1V reference must
	   also be selected for the ADC reference source in the temperature sensor measurement. 
	 */

	ADMUX =
		(1 << REFS1) |     // Sets ref. voltage to 1.1v, bit 1   
		(0 << REFS0) |     // Sets ref. voltage to 1.1v, bit 0
		(1 << MUX5)  |     // use ADC8 for temperature, MUX bit 5
		(0 << MUX4)  |     // use ADC8 for temperature, MUX bit 4
		(0 << MUX3)  |     // use ADC8 for temperature, MUX bit 3
		(0 << MUX2)  |     // use ADC8 for temperature, MUX bit 2
		(1 << MUX1)  |     // use ADC8 for temperature, MUX bit 1
		(0 << MUX0);       // use ADC8 for temperature, MUX bit 0

	ADCSRA = 
		(1 << ADEN)  |     // Enable ADC 
		(1 << ADPS2) |     // set prescaler to 128, bit 2 
		(1 << ADPS1) |     // set prescaler to 128, bit 1 
		(1 << ADPS0);      // set prescaler to 128, bit 0 

	ADCSRB = 
		//(1 << ADLAR);      // left shift result (for 8-bit values)
		(0 << ADLAR);      // right shift result (for 10-bit values)
}
//T = k * [(ADCH << 8) | ADCL] + T_os

uint16_t adc_measure()
{
	ADCSRA |= (1 << ADSC);         // start ADC measurement
	while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 
	uint16_t result = ADCL;
	result |= ADCH<<8;
	return result;
}

uint16_t adc_get_reading()
{
	uint16_t sum=0;  //each reading is 10 bits.  we can sum 2^6 of them
	int i;
	for(i=0;i<32;i++)
		sum+=adc_measure();
	return sum>>5;
}
