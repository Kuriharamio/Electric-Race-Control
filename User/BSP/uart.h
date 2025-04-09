#ifndef __UART_H__
#define __UART_H__

#include "config.h"
#include "Base_Modules/led.h"


typedef struct Class_UART
{
    uint8_t index;     // 串口索引号

    uint8_t current_byte; // 当前接收的字节
    bool is_inited;       // 串口初始化标志位
    bool is_configured;   // 串口配置标志位

    void (*Init)(struct Class_UART *this);                 // 初始化函数
    void (*Send_Bit)(struct Class_UART *this, uint8_t data);                  // 发送数据函数
    void (*Send_Datas)(struct Class_UART *this, uint8_t *datas, size_t size); // 发送字符串函数

    void (*UART_INST_DataProcess)(struct Class_UART *this); // 串口中断处理函数

} Class_UART, *pClass_UART;

pClass_UART Create_UART(uint8_t index);   // 创建UART对象实例
pClass_UART Get_UART_INST(uint8_t index); // 获取UART对象实例

void UART_Init(pClass_UART this);

void UART_Send_Bit(pClass_UART this, uint8_t data);
void UART_Send_Datas(pClass_UART this, uint8_t *datas, size_t size);

IRQn_Type Get_UART_IRQN_From_Index(uint8_t index);
UART_Regs *Get_UART_INST_From_Index(uint8_t index);

#endif // __UART_H__
