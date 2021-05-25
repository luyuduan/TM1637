#include "delay.h"

/*
 * t : 定时时间
 * Ticks : 多少个时钟周期产生一次中断
 * f : 时钟频率 72000000
 * t = Ticks * 1/f = (72000000/1000000) * (1/72000000) = 1us
 */
void delay_us( __IO uint32_t us )
{
    uint32_t i;
    /*Initializes the System Timer and its interrupt, and starts the System Tick Timer.*/
    SysTick_Config( SystemCoreClock / 1000000 );/*设置加载值，并打开中断 */
 
    for( i = 0; i < us; i++ )
    {
        // 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
        while( !( ( SysTick->CTRL ) & ( 1 << 16 ) ) );
    }
    // 关闭SysTick定时器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
 

void delay_ms( __IO uint32_t ms )
{
    uint32_t i;
    SysTick_Config( SystemCoreClock / 1000 );
 
    for( i = 0; i < ms; i++ )
    {
        // 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
        // 当置1时，读取该位会清0
        while( !( ( SysTick->CTRL ) & ( 1 << 16 ) ) );
    }
    // 关闭SysTick定时器
    SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}
