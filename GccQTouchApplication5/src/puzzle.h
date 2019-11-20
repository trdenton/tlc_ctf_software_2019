#ifndef __PUZZLE_H__
#define __PUZZLE_H__
#include <stdbool.h>
bool puzzle_rotor_step(int sensed);
bool puzzle_temp_step(int sensed);
void puzzle_temp_solved(void(*)(unsigned char), void(*)(unsigned long));
void puzzle_rotor_solved(void(*)(unsigned char), void(*)(unsigned long));
void puzzle_rotor_reset(void);
#endif
