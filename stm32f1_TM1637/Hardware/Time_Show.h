#ifndef __TIME_SHOW_H
#define __TIME_SHOW_H

#include <stdint.h>

void Time_Init(uint16_t hour, uint8_t minute);
void Time_Show(void);
void Time_Add_a_second(void);

#endif