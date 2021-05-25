#ifndef __MY_USART1_H
#define __MY_USART1_H

#include "sys.h"	

extern int DEBUG_ERR;
extern int DEBUG_INF;
extern int DEBUG_PRT;

#define debug_inf(msg...) \
do{ \
    if (DEBUG_INF) { \
        char header[50];\
            sprintf(header,"I:%s(%d):",__func__,__LINE__);\
                printf("%s",header); \
					printf(msg); \
    } \
}while(0)

#define debug_err(msg...) \
do{ \
    if (DEBUG_ERR) { \
        char header[50];\
            sprintf(header,"E:%s(%d):",__func__,__LINE__);\
                printf("%s",header); \
					printf(msg); \
    } \
}while(0)

#define myprintf(msg...)\
do{ \
    if (DEBUG_PRT) { \
        printf(msg); \
    } \
}while(0)

void My_USART1_Init(void);
void USART_SendByte(USART_TypeDef* USARTx, uint16_t Data);
void USART_SendStr(USART_TypeDef* USARTx, char *str);
uint16_t USART_ReceiveByte(USART_TypeDef* USARTx);
void USART1_IRQHandler(void);

#endif
