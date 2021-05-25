
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
}; //ֻ���ڵڶ�λ7���������С������ʾ

uint8_t SEGData[][24] =
{
    /*���� ����С���� */
    {0x40 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0e, 0x09, 0x47, 0x48, 0x41, 0x0c, 0x23, 0x3f, 0x7f},
    /*���� ��С���� = ����С���� + 0x80�������λ��ʾ��*/
    {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E, 0x89, 0xC7, 0xC8, 0xC1, 0x8C, 0xA3, 0xBF, 0xFF},
    
    /*���� ����С����*/
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x76, 0x38, 0x37, 0x3E, 0x73, 0x5C, 0x40, 0x00},
    /*���� ��С���� = ����С���� + 0x80*/
    {0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef, 0xf7, 0xfc, 0xb9, 0xde, 0xf9, 0xf1, 0xf6, 0xb8, 0xb7, 0xbe, 0xf3, 0xdc, 0xc0, 0x80},
};
#endif

char mySEGChar[] = {'0'   , '1'   , '2'   , '3'   , '4'   , '5'   , '6'   , '7'   , '8'   , '9'   , '-'   , '\0'
}; 
uint8_t mySEGData[][12] =
{
    /*���� ����С����*/
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x40, 0x00},
    /*���� ��С���� = ����С���� + 0x80*/
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
    
    //��ʼ��ʱ��
    RCC_APB2PeriphClockCmd(TM1637_CLK_RCC_PORT | TM1637_DIO_RCC_PORT, ENABLE);
    //��ʼ������ģʽ
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = TM1637_CLK_GPIO_PIN | TM1637_DIO_GPIO_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TM1637_CLK_GPIO_PORT, &GPIO_InitStruct);
    
    TM1637_clk = 1;
    TM1637_dio = 1;
}

/* �������Ϳ�ʼ�ź� */
void TM1637_Start(void)
{
    /*��������Ŀ�ʼ������ CLKΪ�ߵ�ƽʱ��DIO �ɸ߱�ͣ�*/
    TM1637_clk = 1;
    TM1637_dio = 1;
    delay_us(2);
    TM1637_dio = 0;
}


/*����Ϊ�������룬�ȴ��ӻ�����*/
void TM1637_Set_DIO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = TM1637_DIO_GPIO_PIN;
    GPIO_Init(TM1637_DIO_GPIO_PORT, &GPIO_InitStruct);
    
}

/* ��������Ϊ����������������� */
void TM1637_Set_DIO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = TM1637_DIO_GPIO_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TM1637_DIO_GPIO_PORT, &GPIO_InitStruct);
    
}

/* ��������Ӧ�����������ȴ��ӻ�Ӧ���ź� */
int TM1637_WaitAck(void)
{
    /*  TM1637 �����ݴ������Ӧ���ź� ACK��������������ȷʱ��
    ���ڵڰ˸�ʱ�ӵ��½��أ�оƬ�ڲ������һ��Ӧ���ź� ACK �� DIO �ܽ����ͣ�
    �ڵھŸ�ʱ�ӽ���֮���ͷ� DIO �ߡ�
    */
    int status = 0;
    
    TM1637_Set_DIO_IN();

    TM1637_clk = 0;/*�ڰ˸�ʱ�ӵ��½���*/
    delay_us(5);
    TM1637_clk = 1;/*�ھŸ�ʱ�ӵ�������*/

    delay_us(5);
    TM1637_clk = 0;/*�ھŸ�ʱ�ӵ��½���*/
    if(TM1637_dio_in == 1){
        status = -1;
    }
    
    TM1637_Set_DIO_OUT();
    
    return status;
}


void TM1637_Stop(void)
{
    /* ���������� CLK Ϊ��ʱ��DIO �ɵ͵�ƽ��Ϊ�ߵ�ƽ�� */
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
        /*  ����������ʱ��
        �� CLK �Ǹߵ�ƽʱ��DIO �ϵ��źű��뱣�ֲ��䣻
        ֻ�� CLK �ϵ�ʱ���ź�Ϊ�͵�ƽʱ��DIO �ϵ��źŲ��ܸı䡣
        */
        TM1637_clk = 0; /*CLK �ϵ�ʱ���ź�Ϊ�͵�ƽ*/
        
        /*DIO �ϵ��źŲ��ܸı�*/
        
        TM1637_dio =  oneByte & 0x01;
        //myprintf("%d ", TM1637_dio);
        delay_us(3);
        TM1637_clk = 1; /*�� CLK �Ǹߵ�ƽʱ��DIO �ϵ��źű��뱣�ֲ��䣻*/
        delay_us(5);
        oneByte = oneByte >> 1;
    }
    
    /*�ؼ���WaitACKǰ����������ʱ���ߣ�����Ϳ��ܷ����˽����ź�*/
    TM1637_clk = 0; 

    return TM1637_WaitAck();
}



// level : �������ȵȼ�  0---7
void TM1637_SetBrightness( uint8_t level )
{
    Brightness &= !0x07;
    Brightness |= level;
}


void TM1637_writeCammand(uint8_t Cammand)
{
    TM1637_Start();
    TM1637_WriteByte(Cammand); // 40H ��ַ�Զ��� 1 ģʽ,44H �̶���ַģʽ,����������Լ� 1 ģʽ
    TM1637_Stop();
    
}

/*�ĸ������0xc0,0xc1,0xc2,0xc3*/
void TM1637_writeData(uint8_t addr, uint8_t mData)
{
    TM1637_Start();
    TM1637_WriteByte(addr);  //��ַ
    TM1637_WriteByte(mData);  //����
    TM1637_Stop();
}


/*�ĸ������0xc0,0xc1,0xc2,0xc3
    �������Ѷ�Ӧ

*/
void TM1637_SmgDisplay_40(uint8_t *const discode)
{
    uint8_t i;
    uint8_t mData;
    TM1637_writeCammand(DATA_CMD | DATA_40);
    
    TM1637_Start();
    TM1637_WriteByte(START_ADDR); //�����׵�ַ��
    for (i = 0; i < 4; i++) //��ַ�Լӣ�����ÿ�ζ�д��ַ
    {
        mData = mySEGData[0][*(discode + i)];
        TM1637_WriteByte(mData); //��������
    }
    TM1637_Stop();
    
    
    /*������ʾ��������
    �������뿪��
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
    /*40ģʽ��ʹ��*/
    myprintf("40ģʽ\r\n");
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
    /*44ģʽ��ʹ��*/
    myprintf("44ģʽ\r\n");
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
    /*��2λ��С�����ʹ��*/
    myprintf("С����\r\n");
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
    /*���Ȳ���*/
    myprintf("���Ȳ���\r\n");
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








