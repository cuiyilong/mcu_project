#include "sys.h"

#include "delay.h"
#include "usart.h"

/*********************************************************************************
 * 文件名  ：main.c
 * 描述    ：通过stm32的spi1读取max6675的温度值，并通过uart1发送出来
 *          
 * 实验平台：STM32开发板
 * 库版本  ：ST3.0.0
 * 硬件连接： ------------------------------------
 *           |PA6-SPI1-MISO：MAX6675-SO          |
 *           |PA7-SPI1-MOSI：MAX6675-SI          |
 *           |PA5-SPI1-SCK ：MAX6675-SCK         |
 *           |PA4-SPI1-NSS ：MAX6675-CS          |
 *            ------------------------------------
**********************************************************************************/

#define	MAX6675_CS 		PAout(4)  		//W25QXX的片选信号
#define 	MAX6675_CSL()		MAX6675_CS = 0;
#define 	MAX6675_CSH()		MAX6675_CS = 1;

/*
 * 函数名：SPI1_Init
 * 描述  MAX6675 接口初始化
 * 输入  ：无 
 * 输出  ：无
 * 返回  ：无
 */																						  
void SPI_MAX6675_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;	

    /* 使能 SPI1 时钟 */       

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟

    /* ---------通信I/O初始化----------------
    * PA5-SPI1-SCK :MAX6675_SCK
    * PA6-SPI1-MISO:MAX6675_SO
    * PA7-SPI1-MOSI:MAX6675_SI	 
    */


    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //GPIOA5复用为SPI1-CLK
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //GPIOA6复用为SPI1-MISO
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //GPIOA7复用为SPI1-MOSI


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ---------控制I/O初始化----------------*/
    /* PA4-SPI1-NSS:MAX6675_CS */							// 片选
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		// 推免输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);						  
    GPIO_SetBits(GPIOA, GPIO_Pin_4);						// 先把片选拉高，真正用的时候再拉低


    /* SPI1 配置 */ 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);


    /* 使能 SPI1  */
    SPI_Cmd(SPI1, ENABLE); 
}

/*
 *
 *
 *
 */
unsigned char MAX6675_ReadByte(void)
{
	
    /* Loop while DR register in not emplty */
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, 0xff);

    /* Wait to receive a byte */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}
/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无	
 */
 max_6675_init(void)
{

    /* MAX6675 SPI 接口初始化 */
    SPI_MAX6675_Init();

    printf("\r\n max_6675_init!!!\r\n");
}

float f_temprature = 0;
float max_6675_temp_detect(void)
{
    unsigned int t,i;
    unsigned char c;
    unsigned char flag;
    float temprature = 0;

    MAX6675_CSL();
    c = MAX6675_ReadByte();
    i = c;
    i = i<<8;
    c = MAX6675_ReadByte();
    MAX6675_CSH();

    i = i|((unsigned int)c);			//i是读出来的原始数据
    flag = i&0x04;						//flag保存了热电偶的连接状态
    t = i<<1;
    t = t>>4;
    temprature = t*0.25;
    if(i!=0)							//max6675有数据返回
    {
    if(flag==0)						//热电偶已连接
    {
        printf("原始数据是：%04X,  当前温度是：%4.2f。\r\n",i,temprature);
        f_temprature = temprature;
        return temprature;
    }	
    else							//热电偶掉线
    {
        printf("未检测到热电偶，请检查。\r\n");
        return 0;
    }

    }
    else								//max6675没有数据返回
    {
        printf("max6675没有数据返回，请检查max6675连接。\r\n");
        return 0;
    }
    //max6675的转换时间是0.2秒左右，所以两次转换间隔不要太近    
    delay_ms(200);
    return temprature;
}