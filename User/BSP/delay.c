#include "BSP/delay.h"

// volatile unsigned int delay_times_us = 0;
// void delay_us(unsigned long us)
// {
//     delay_times_us = us;
//     while (delay_times_us != 0)
//     {
//     }
// }

// void delay_ms(unsigned long ms)
// {
//     delay_times_us = ms * 1000;
//     while (delay_times_us != 0)
//     {
//     }
// }

// // 滴答定时器中断服务函数
// void SysTick_Handler(void)
// {
//     if (delay_times_us != 0)
//     {
//         delay_times_us--;
//     }
// }

volatile unsigned int delay_times_ms = 0;
volatile unsigned int system_time_ms = 0;

void delay_ms(unsigned long ms)
{
    delay_times_ms = ms;
    while (delay_times_ms != 0)
    {
    }
}

// 滴答定时器中断服务函数
void SysTick_Handler(void)
{
    system_time_ms++;
    if (delay_times_ms != 0)
    {
        delay_times_ms--;
    }
}
