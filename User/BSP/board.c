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

void softwareReset(uint32_t resetType)
{
    DL_SYSCTL_resetDevice(SYSCTL_RESETLEVEL_LEVEL_POR);
}

void board_init(void)
{
    // SYSCFG初始化
    SYSCFG_DL_init();

    // TIM初始化
    // NVIC_ClearPendingIRQ(ENCODER_INST_INT_IRQN);
    // NVIC_ClearPendingIRQ(PID_INST_INT_IRQN);
    // NVIC_ClearPendingIRQ(ADC_BUTTON_INST_INT_IRQN);

    // NVIC_EnableIRQ(ENCODER_INST_INT_IRQN);
    // NVIC_EnableIRQ(PID_INST_INT_IRQN);
    // NVIC_EnableIRQ(ADC_BUTTON_INST_INT_IRQN);

    // GPIO初始化
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
}
