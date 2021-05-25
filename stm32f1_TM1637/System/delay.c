#include "delay.h"

/*
 * t : ��ʱʱ��
 * Ticks : ���ٸ�ʱ�����ڲ���һ���ж�
 * f : ʱ��Ƶ�� 72000000
 * t = Ticks * 1/f = (72000000/1000000) * (1/72000000) = 1us
 */
void delay_us( __IO uint32_t us )
{
    uint32_t i;
    /*Initializes the System Timer and its interrupt, and starts the System Tick Timer.*/
    SysTick_Config( SystemCoreClock / 1000000 );/*���ü���ֵ�������ж� */
 
    for( i = 0; i < us; i++ )
    {
        // ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
        while( !( ( SysTick->CTRL ) & ( 1 << 16 ) ) );
    }
    // �ر�SysTick��ʱ��
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
 

void delay_ms( __IO uint32_t ms )
{
    uint32_t i;
    SysTick_Config( SystemCoreClock / 1000 );
 
    for( i = 0; i < ms; i++ )
    {
        // ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
        // ����1ʱ����ȡ��λ����0
        while( !( ( SysTick->CTRL ) & ( 1 << 16 ) ) );
    }
    // �ر�SysTick��ʱ��
    SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}
