#include "BSP/board.h"

#if !defined(__MICROLIB)
#if (__ARMCLIB_VERSION <= 6000000)
struct __FILE
{
    int handle;
};
#endif

void _sys_exit(int x)
{
    x = x;
}
#endif

#ifdef USE_BLUETOOTH
int fputc(int ch, FILE *stream)
{
    Get_UART_INST(BLUETOOTH_UART_INDEX)->Send_Bit(Get_UART_INST(BLUETOOTH_UART_INDEX), (uint8_t)ch); 
    return ch;
}
#endif

void softwareReset(void)
{
    DL_SYSCTL_resetDevice(SYSCTL_RESETLEVEL_LEVEL_POR);
}

void Enable_All_Interrupt(void)
{
    //* TIM初始化
#ifdef USE_PID
    NVIC_ClearPendingIRQ(PID_TIMER_INST_INT_IRQN);
    NVIC_EnableIRQ(PID_TIMER_INST_INT_IRQN);
#endif
#if defined(USE_ADC_BUTTON) || defined(USE_GRAY_SENSOR)
    NVIC_ClearPendingIRQ(READ_TIMER_INST_INT_IRQN);
    NVIC_EnableIRQ(READ_TIMER_INST_INT_IRQN);
#endif
#ifdef USE_PTZ
    NVIC_ClearPendingIRQ(PTZ_TIMER_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(PTZ_TIMER_INST_INT_IRQN);
#endif
#ifdef USE_ENCODER
    NVIC_ClearPendingIRQ(ENCODER_TIMER_INST_INT_IRQN);
    NVIC_EnableIRQ(ENCODER_TIMER_INST_INT_IRQN);
    //* GPIO初始化
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
#endif


}
