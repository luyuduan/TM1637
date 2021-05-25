//#include "sys.h"	
#include "stdio.h"
#include "stm32f10x.h"
/************************
1.串口时钟、GPIO时钟使能
2.GPIO端口模式设置，PA9-TX-复用推挽输出，PA10-RX-浮空输入
3.串口参数初始化
4.开启中断，初始化NVIC
5.使能串口
6.编写中断服务程序

************************/

void My_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//1.串口时钟、GPIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	//2.GPIO端口模式设置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStruct);//PA9-TX-复用推挽输出
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init( GPIOA, &GPIO_InitStruct);//PA10-RX-浮空输入
	
	//3.串口参数初始化
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控；
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//支持收发
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验位；
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位1位；
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; //数据位8位；
	USART_Init(USART1, &USART_InitStruct);
	
	//4.开启中断，初始化NVIC
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct); //初始化NVIC
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开中断
	
	//5.使能串口
	USART_Cmd(USART1, ENABLE);
	
	//6.编写中断处理函数
	//void USART1_IRQHandler(void)
		
}


void USART_SendByte(USART_TypeDef* USARTx, uint16_t Data)
{
	 USART_SendData(USARTx, Data);
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//新加,保证数据不覆盖，TDR为空，数据已到发送移位寄存器
}


void USART_SendStr(USART_TypeDef* USARTx, char *str)//接受char类型指针，方便操作
{
	while(*str != '\0')
	{
		USART_SendByte(USARTx, *str++);//字符串就是数组，数组名是首地址，*原数据，指针++
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//保证发送移位寄存器也已空，数据完全发送到引脚
}

uint16_t USART_ReceiveByte(USART_TypeDef* USARTx)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);//没有收到数据，一直等待
	return USART_ReceiveData(USART1);//返回接收数据
}

void USART1_IRQHandler(void)//可以写在stm32f10x_it.c里
{
	u16 temp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)//
	{
		temp = USART_ReceiveData(USART1);
		USART_SendByte(USART1, temp);
		USART_SendStr(USART1, "1123");
	}

}

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (unsigned char) ch);// USART1 可以换成 USART2 等
    
    while (!(USART1->SR & USART_FLAG_TXE));
    
    return (ch);
    
}
