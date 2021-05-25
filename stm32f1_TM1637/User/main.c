#include "stdio.h"
#include "TM1637.h"
#include "delay.h"
#include "usart.h"
#include "Time_Show.h"

int DEBUG_PRT=1;
int DEBUG_ERR=1;
int DEBUG_INF=1;

int main(void)
{
    My_USART1_Init();
    printf("APP START\r\n");
    
    tm1637_Init();
    ShowTest();
    
    Time_Init(12,0);
    TM1637_SetBrightness(level2);
    
	while(1){
        delay_us(1000000-11000-120);
        Time_Add_a_second();
    }
    
}
