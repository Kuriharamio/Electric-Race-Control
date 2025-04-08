#include "BSP/delay.h"
// 搭配滴答定时器实现的精确us延时
void delay_us(unsigned long __us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 38;

    // 计算需要的时钟数 = 延迟微秒数 * 每微秒的时钟数
    ticks = __us * (32000000 / 1000000);

    // 获取当前的SysTick值
    told = SysTick->VAL;

    while (1)
    {
        // 重复刷新获取当前的SysTick值
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            // 如果达到了需要的时钟数，就退出循环
            if (tcnt >= ticks)
                break;
        }
    }
}

volatile unsigned int delay_times = 0;
// 搭配滴答定时器实现的精确ms延时
void delay_ms(unsigned long ms)
{
    delay_times = ms;
    while (delay_times != 0)
        ;
}

// 滴答定时器中断服务函数
void SysTick_Handler(void)
{
    if (delay_times != 0)
    {
        delay_times--;
    }
}
