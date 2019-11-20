#ifndef __ADC_H__
#define __ADC_H__
#include <stdint.h>
void adc_init(void);
uint16_t adc_measure(void);
uint16_t adc_get_reading(void);
#endif
