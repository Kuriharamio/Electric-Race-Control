#include "BSP/board.h"

#if !defined(__MICROLIB)
// 不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
// 如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
    int handle;
};
#endif

// FILE __stdout;

// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
#endif

void board_init(void)
{
    // SYSCFG初始化
    SYSCFG_DL_init();

    // 串口初始化
    pClass_UART uart_0 = Create_UART(0);
    uart_0->Init(uart_0, STREAM); // 初始化串口0
    uart_0 = NULL;        // 释放串口0对象

    // ADC初始化

    // GPIO初始化
}
