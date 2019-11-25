#include "sys.h"

#include "delay.h"
#include "usart.h"

/* pin desc*/
// 12-PA12   9-PA9  8-PA8   6-PA6    L----R  0 1 2 3


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

   // GPIO_ResetBits(GPIOD,GPIO_Pin_All);//GPIOF9,F10设置高，灯灭

   //GPIO_SetBits(GPIOD,GPIO_Pin_All && (~(GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11)));

   //GPIO_ResetBits(GPIOD, GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11);

    GPIO_SetBits(GPIOD,GPIO_Pin_All);
   
    //GPIO_ResetBits(GPIOD, GPIO_Pin_12);

}

//
//共阴数码管对应的编码表

//pin map
//  1 ----6:   e(1) d(2) dp(3) c(4) g(5) dig4(6)
// 12 ----7: dig1(12) a(11) f(10) dig2(9) dig3(8) b(7)


u8 digit_select_table[5]={0, 9, 10, 11, 12}; // 6,8,9,12
u8 num_table[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; 
//PA0-PA7----A-G-D0
#define DIGIT_BIT_NUM 8
#define DISPLY_BITS (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6)
#define DISPLY_DIGS (GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12)

void digital_tube_display_bit(u32 digit_bits)
{
    GPIO_ResetBits(GPIOD, DISPLY_BITS);
    //delay_ms(1);   
    GPIO_SetBits(GPIOD, digit_bits);
       
}
void digital_tube_display_one(u32 digit_select, u32 display_digit)
{
    assert_param(display_digit < 10);
    
    
    printf("\r\n digit_select %d   display_digit %d   num_table[display_digit] 0x%02x \r\n",
                    digit_select, display_digit, num_table[display_digit]);
    digital_tube_display_bit(num_table[display_digit]);

    GPIO_SetBits(GPIOD, DISPLY_DIGS);
    //delay_ms(1); 
    GPIO_ResetBits(GPIOD, BIT(digit_select_table[digit_select]));
    //delay_ms(5); 


}

#define DISPLAY_DIGITS 4
void digital_tube_display(u32 dis_num)
{
    u32 i;
    u32 display_digit;

    for(i = 0; i < DISPLAY_DIGITS; i++) {
            display_digit = dis_num%10;
            dis_num /= 10;

            printf("\r\n dis_num %d   display_digit %d   \r\n", dis_num, display_digit);
            digital_tube_display_one(i+1, display_digit);
    }

}
