#include "sys.h"

#include "delay.h"
#include "usart.h"

//共阴数码管对应的编码表

//pin map
//  1 ----6:   e(1) d(2) dp(3) c(4) g(5) dig4(6)
// 12 ----7: dig1(12) a(11) f(10) dig2(9) dig3(8) b(7)

u8 digit_select_table[5]={0, 9, 10, 11, 12}; // 6,8,9,12
u8 num_table[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; 
//PA0-PA7----A-G-D0
#define DIGIT_BIT_NUM 8
#define DISPLAY_DIGITS 4
#define DISPLY_BITS (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6)
#define DISPLY_DIGS (GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12)

#define BIT(l) (1<< l)

/*digital tube use PD0-PD15 */
void digital_tube_Init(void)
{    	 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化

    GPIO_SetBits(GPIOD,GPIO_Pin_All);
}

void digital_tube_display_bit(u32 digit_bits)
{
    GPIO_ResetBits(GPIOD, DISPLY_BITS);

    GPIO_SetBits(GPIOD, digit_bits);
       
}

void digital_tube_display_one(u32 digit_select, u32 display_digit, u32 display_decimal)
{
    u32 digit_bits = 0;
    assert_param(display_digit < 10);
 
    printf("\r\n digit_select %d   display_digit %d   num_table[display_digit] 0x%02x  display_decimal %d\r\n",
                    digit_select, display_digit, num_table[display_digit], display_decimal);
    digit_bits = num_table[display_digit];
    if (display_decimal)
        digit_bits |= BIT(7);
    digital_tube_display_bit(digit_bits);

    GPIO_SetBits(GPIOD, DISPLY_DIGS);

    GPIO_ResetBits(GPIOD, BIT(digit_select_table[digit_select]));
}

void digital_tube_display(float dis_num)
{
    u32 i;
    u32 display_digit;
    u32 display_decimal[DISPLAY_DIGITS] = {0};

    u32 dis_num_multy = dis_num;

    if(dis_num > 9999) {
        dis_num = 9999;
    } else if(dis_num >= 100 && dis_num < 1000) {
        display_decimal[1] = 1;
        dis_num_multy = dis_num *10;

    } else if(dis_num >= 10 && dis_num < 100) {
        display_decimal[2] = 1;
        dis_num_multy = dis_num *100;

    } else if(dis_num >= 0 && dis_num < 10) {
        display_decimal[3] = 1;
        dis_num_multy = dis_num *1000;
    }

    printf("\r\n dis_num_multy %d  dis_num %f  \r\n", dis_num_multy,dis_num);
        
    for(i = 0; i < DISPLAY_DIGITS; i++) {
            display_digit = dis_num_multy%10;
            dis_num_multy /= 10;
            digital_tube_display_one(i+1, display_digit, display_decimal[i]);
    }
}
