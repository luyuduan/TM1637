#ifndef __TM1637_H
#define __TM1637_H

#include <stdint.h>
enum{
    level0=0,
    level1,
    level2,
    level3,
    level4,
    level5,
    level6,
    level7,
    level8,
};

void tm1637_Init(void);
void TM1637_SetBrightness( uint8_t level );
void TM1637_Clear_Show(void);
void TM1637_SmgDisplay_40(uint8_t *discode);
void TM1637_SmgDisplay_44(uint8_t addr, uint8_t mData, uint8_t ifDot);
void ShowTest(void);


#endif

