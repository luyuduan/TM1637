//#include "sys.h"	
#include "stdio.h"
#include "stm32f10x.h"
/************************
1.����ʱ�ӡ�GPIOʱ��ʹ��
2.GPIO�˿�ģʽ���ã�PA9-TX-�������������PA10-RX-��������
3.���ڲ�����ʼ��
4.�����жϣ���ʼ��NVIC
5.ʹ�ܴ���
6.��д�жϷ������

************************/

void My_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//1.����ʱ�ӡ�GPIOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	//2.GPIO�˿�ģʽ����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStruct);//PA9-TX-�����������
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init( GPIOA, &GPIO_InitStruct);//PA10-RX-��������
	
	//3.���ڲ�����ʼ��
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ�����أ�
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//֧���շ�
	USART_InitStruct.USART_Parity = USART_Parity_No;//��У��λ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//ֹͣλ1λ��
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; //����λ8λ��
	USART_Init(USART1, &USART_InitStruct);
	
	//4.�����жϣ���ʼ��NVIC
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct); //��ʼ��NVIC
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
	
	//5.ʹ�ܴ���
	USART_Cmd(USART1, ENABLE);
	
	//6.��д�жϴ�����
	//void USART1_IRQHandler(void)
		
}


void USART_SendByte(USART_TypeDef* USARTx, uint16_t Data)
{
	 USART_SendData(USARTx, Data);
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�¼�,��֤���ݲ����ǣ�TDRΪ�գ������ѵ�������λ�Ĵ���
}


void USART_SendStr(USART_TypeDef* USARTx, char *str)//����char����ָ�룬�������
{
	while(*str != '\0')
	{
		USART_SendByte(USARTx, *str++);//�ַ����������飬���������׵�ַ��*ԭ���ݣ�ָ��++
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//��֤������λ�Ĵ���Ҳ�ѿգ�������ȫ���͵�����
}

uint16_t USART_ReceiveByte(USART_TypeDef* USARTx)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);//û���յ����ݣ�һֱ�ȴ�
	return USART_ReceiveData(USART1);//���ؽ�������
}

void USART1_IRQHandler(void)//����д��stm32f10x_it.c��
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
    USART_SendData(USART1, (unsigned char) ch);// USART1 ���Ի��� USART2 ��
    
    while (!(USART1->SR & USART_FLAG_TXE));
    
    return (ch);
    
}
