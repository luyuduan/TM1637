
#include "TM1637.h"
#include "Time_Show.h"
#include "stdio.h"
#include "usart.h"
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    
}sTIME;

sTIME s_time;


void Time_Init(uint16_t hour, uint8_t minute)
{
    if(hour>24){
        debug_err("Error: hour=%d\r\n", hour);
        hour = 0;
        minute = 0;
    }
    if(minute>60){
        debug_err("Error: minute=%d\r\n", minute);
        hour = 0;
        minute = 0;
    }
    
    s_time.hour = hour;
    s_time.minute = minute;
    Time_Show();
}


void Time_Add_a_second(void)
{
    uint8_t bit_1;
    
    debug_inf("time:%02d:%02d:%02d\r\n", s_time.hour, s_time.minute, s_time.second);
    s_time.second++;
    
    /*冒号闪烁*/
    bit_1 = s_time.hour%10;
    TM1637_SmgDisplay_44(1, bit_1, s_time.second%2);
    myprintf("\r\n");
    /*时间分钟级更新*/
    if(s_time.second>=60){
        s_time.second=0;
        s_time.minute++;
        if(s_time.minute>=60)
        {
            s_time.minute=0;
            s_time.hour++;
            if(s_time.hour>=24)
            {
                s_time.hour=0;
            }
        }
        /*一分钟就该更新显示*/
        Time_Show();
    }
    
}


void Time_Show(void)
{
    uint8_t time_list[4];
    
    time_list[0] = s_time.hour/10 ;
    time_list[1] = s_time.hour%10 ;
    time_list[2] = s_time.minute/10 ;
    time_list[3] = s_time.minute%10 ;
    
    TM1637_SmgDisplay_40(time_list);
}







