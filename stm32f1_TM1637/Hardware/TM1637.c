
#include "stm32f10x_gpio.h"
#include "sys.h"
#include "delay.h"
#include "TM1637.h"

#define START_ADDR 0xc0

#define DATA_CMD 0x40
#define DATA_44 0x04
#define DATA_40 0x00

#define ADDR_CMD 0xC0
#define ADDR_00 0x00
#define ADDR_01 0x01
#define ADDR_02 0x02
#define ADDR_03 0x03

#define SHOW_CMD 0x80
#define SHOW_ON 0x08

uint8_t Brightness = 0x8f;

#if 0
char SEGChar[] = {'0' , '1'   , '2'   , '3'   , '4'   , '5'   , '6'   , '7'   , '8'   , '9'   , 'A'   , 'B'   , 'C'   , 'D'   , 'E'   , 'F'   , 'H'   , 'L'   , 'n'   , 'u'   , 'P'   , 'o'   , '-'   , '\0'
}; //只用于第二位7段数码管有小数点显示

uint8_t SEGData[][24] =
{
    /*共阳 不带小数点 */
    {0x40 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0e, 0x09, 0x47, 0x48, 0x41, 0x0c, 0x23, 0x3f, 0x7f},
    /*共阳 带小数点 = 不带小数点 + 0x80，即最高位表示点*/
    {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E, 0x89, 0xC7, 0xC8, 0xC1, 0x8C, 0xA3, 0xBF, 0xFF},
    
    /*共阴 不带小数点*/
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x76, 0x38, 0x37, 0x3E, 0x73, 0x5C, 0x40, 0x00},
    /*共阴 带小数点 = 不带小数点 + 0x80*/
    {0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef, 0xf7, 0xfc, 0xb9, 0xde, 0xf9, 0xf1, 0xf6, 0xb8, 0xb7, 0xbe, 0xf3, 0xdc, 0xc0, 0x80},
};
#endif

char mySEGChar[] = {'0'   , '1'   , '2'   , '3'   , '4'   , '5'   , '6'   , '7'   , '8'   , '9'   , '-'   , '\0'
}; 
uint8_t mySEGData[][12] =
{
    /*共阴 不带小数点*/
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x40, 0x00},
    /*共阴 带小数点 = 不带小数点 + 0x80*/
    {0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef, 0xc0, 0x80},
};

void tm1637_GPIO_Init(void);

void tm1637_Init(void)
{
    tm1637_GPIO_Init();
    TM1637_Clear_Show();
}

/*
PB10 -- CLK
PB11 -- DIO
*/

#define TM1637_CLK_GPIO_PORT        GPIOB
#define TM1637_CLK_GPIO_PIN         GPIO_Pin_10
#define TM1637_CLK_RCC_PORT         RCC_APB2Periph_GPIOB

#define TM1637_DIO_GPIO_PORT        GPIOB
#define TM1637_DIO_GPIO_PIN         GPIO_Pin_11
#define TM1637_DIO_RCC_PORT         RCC_APB2Periph_GPIOB

#define TM1637_clk PBout(10)
#define TM1637_dio PBout(11)
#define TM1637_dio_in PBin(11)

void tm1637_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    //初始化时钟
    RCC_APB2PeriphClockCmd(TM1637_CLK_RCC_PORT | TM1637_DIO_RCC_PORT, ENABLE);
    //初始化引脚模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = TM1637_CLK_GPIO_PIN | TM1637_DIO_GPIO_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TM1637_CLK_GPIO_PORT, &GPIO_InitStruct);
    
    TM1637_clk = 1;
    TM1637_dio = 1;
}

/* 主机发送开始信号 */
void TM1637_Start(void)
{
    /*数据输入的开始条件是 CLK为高电平时，DIO 由高变低；*/
    TM1637_clk = 1;
    TM1637_dio = 1;
    delay_us(2);
    TM1637_dio = 0;
}


/*设置为上拉输入，等待从机拉低*/
void TM1637_Set_DIO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = TM1637_DIO_GPIO_PIN;
    GPIO_Init(TM1637_DIO_GPIO_PORT, &GPIO_InitStruct);
    
}

/* 重新设置为推挽输出，发送数据 */
void TM1637_Set_DIO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = TM1637_DIO_GPIO_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TM1637_DIO_GPIO_PORT, &GPIO_InitStruct);
    
}

/* 主机创造应答条件，并等待从机应答信号 */
int TM1637_WaitAck(void)
{
    /*  TM1637 的数据传输带有应答信号 ACK，当传输数据正确时，
    会在第八个时钟的下降沿，芯片内部会产生一个应答信号 ACK 将 DIO 管脚拉低，
    在第九个时钟结束之后释放 DIO 线。
    */
    int status = 0;
    
    TM1637_Set_DIO_IN();

    TM1637_clk = 0;/*第八个时钟的下降沿*/
    delay_us(5);
    TM1637_clk = 1;/*第九个时钟的上升沿*/

    delay_us(5);
    TM1637_clk = 0;/*第九个时钟的下降沿*/
    if(TM1637_dio_in == 1){
        status = -1;
    }
    
    TM1637_Set_DIO_OUT();
    
    return status;
}


void TM1637_Stop(void)
{
    /* 结束条件是 CLK 为高时，DIO 由低电平变为高电平。 */
    TM1637_clk = 0;
    TM1637_dio = 0;
    delay_us(2);
    TM1637_clk = 1;
    delay_us(2);
    TM1637_dio = 1;

}


int TM1637_WriteByte(uint8_t oneByte)
{
    uint8_t i;
    
    myprintf("%02x ",oneByte);
    for (i = 0; i < 8; i++) {
        /*  在输入数据时，
        当 CLK 是高电平时，DIO 上的信号必须保持不变；
        只有 CLK 上的时钟信号为低电平时，DIO 上的信号才能改变。
        */
        TM1637_clk = 0; /*CLK 上的时钟信号为低电平*/
        
        /*DIO 上的信号才能改变*/
        
        TM1637_dio =  oneByte & 0x01;
        //myprintf("%d ", TM1637_dio);
        delay_us(3);
        TM1637_clk = 1; /*当 CLK 是高电平时，DIO 上的信号必须保持不变；*/
        delay_us(5);
        oneByte = oneByte >> 1;
    }
    
    /*关键，WaitACK前必须先拉低时钟线，否则就可能发出了结束信号*/
    TM1637_clk = 0; 

    return TM1637_WaitAck();
}



// level : 设置亮度等级  0---7
void TM1637_SetBrightness( uint8_t level )
{
    Brightness &= !0x07;
    Brightness |= level;
}


void TM1637_writeCammand(uint8_t Cammand)
{
    TM1637_Start();
    TM1637_WriteByte(Cammand); // 40H 地址自动加 1 模式,44H 固定地址模式,本程序采用自加 1 模式
    TM1637_Stop();
    
}

/*四个数码管0xc0,0xc1,0xc2,0xc3*/
void TM1637_writeData(uint8_t addr, uint8_t mData)
{
    TM1637_Start();
    TM1637_WriteByte(addr);  //地址
    TM1637_WriteByte(mData);  //数据
    TM1637_Stop();
}


/*四个数码管0xc0,0xc1,0xc2,0xc3
    码与数已对应

*/
void TM1637_SmgDisplay_40(uint8_t *const discode)
{
    uint8_t i;
    uint8_t mData;
    TM1637_writeCammand(DATA_CMD | DATA_40);
    
    TM1637_Start();
    TM1637_WriteByte(START_ADDR); //设置首地址，
    for (i = 0; i < 4; i++) //地址自加，不必每次都写地址
    {
        mData = mySEGData[0][*(discode + i)];
        TM1637_WriteByte(mData); //发送数据
    }
    TM1637_Stop();
    
    
    /*发送显示控制命令
    即亮暗与开关
    */
    TM1637_writeCammand(SHOW_CMD | SHOW_ON | Brightness);
    myprintf("\r\n");
}

/*
addr: 0,1,2,3
if_NoDot: 0-Nodot 1-Dot
*/
void TM1637_SmgDisplay_44(uint8_t addr, uint8_t mData, uint8_t ifDot)
{
    
    TM1637_writeCammand(DATA_CMD | DATA_44);
    
    TM1637_writeData(START_ADDR + addr, mySEGData[ifDot][mData]);
    
    TM1637_writeCammand(SHOW_CMD | SHOW_ON | Brightness);
    
}


void TM1637_Clear_Show(void)
{
    uint8_t num_list[4];
    uint8_t i;
    myprintf("Clear Show\r\n");
    for (i = 0; i < 4; i++) {
        num_list[i] = 11;
    }
    TM1637_SmgDisplay_40(num_list);
    
}

#define TEST_40 0
#define TEST_44 0
#define TEST_PO 0
#define TEST_BR 1
void ShowTest(void)
{
    uint8_t i, j;
    uint8_t num_list[4];
    TM1637_SetBrightness(level2);
    
#if TEST_40
    /*40模式的使用*/
    myprintf("40模式\r\n");
    for (i = 0; i < sizeof(mySEGData[0]); i++) {
        myprintf("char:%c\r\n", mySEGChar[i]);
        for (j = 0; j < 4; j++) {
            num_list[j] = i;
        }
        TM1637_SmgDisplay_40(num_list);
        myprintf("\r\n");
        delay_ms(1000);
    }
#endif
    
#if TEST_44
    /*44模式的使用*/
    myprintf("44模式\r\n");
    j = 0;
    for (i = 0; i < sizeof(mySEGData[0]); i++) {
        myprintf("char:%c \r\n", mySEGChar[i]);
        TM1637_SmgDisplay_44(j, i,0);
        delay_ms(1000);
        myprintf("\r\n");
        j++;
        j = j % 4;
    }
#endif
    
#if TEST_PO
    /*第2位的小数点的使用*/
    myprintf("小数点\r\n");
    TM1637_Clear_Show();
    for (i = 0; i < sizeof(mySEGData[0]); i++) {
        myprintf("char:%c \r\n", mySEGChar[i]);
        
        TM1637_SmgDisplay_44(1, i, 0);
        delay_ms(1000);
        
        myprintf("\r\n");
        
        TM1637_SmgDisplay_44(1, i, 1);
        delay_ms(1000);
        
        myprintf("\r\n");
    }
#endif
    
#if TEST_BR
    /*亮度测试*/
    myprintf("亮度测试\r\n");
    for (j = 0; j < 4; j++) {
        num_list[j] = 8;
    }
    for (i = 0; i < 8;i++){
        TM1637_SetBrightness(i);
        myprintf("Brightness:%d/8\r\n", i+1);
        TM1637_SmgDisplay_40(num_list);
        
        delay_ms(1000);
    }
#endif
    
}








