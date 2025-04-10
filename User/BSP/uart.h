// #ifndef __UART_H__
// #define __UART_H__

// #include "config.h"
// #include "Base_Modules/led.h"

// typedef struct Class_UART
// {
//     uint8_t index;     // 串口索引号

//     uint8_t current_byte; // 当前接收的字节
//     bool is_inited;       // 串口初始化标志位
//     bool is_configured;   // 串口配置标志位

//     void (*Init)(struct Class_UART *this);                 // 初始化函数
//     void (*Send_Bit)(struct Class_UART *this, uint8_t data);                  // 发送数据函数
//     void (*Send_Datas)(struct Class_UART *this, uint8_t *datas, size_t size); // 发送字符串函数

//     void (*UART_INST_DataProcess)(struct Class_UART *this); // 串口中断处理函数

// } Class_UART, *pClass_UART;

// pClass_UART Create_UART(uint8_t index);   // 创建UART对象实例
// pClass_UART Get_UART_INST(uint8_t index); // 获取UART对象实例

// void UART_Init(pClass_UART this);

// void UART_Send_Bit(pClass_UART this, uint8_t data);
// void UART_Send_Datas(pClass_UART this, uint8_t *datas, size_t size);

// IRQn_Type Get_UART_IRQN_From_Index(uint8_t index);
// UART_Regs *Get_UART_INST_From_Index(uint8_t index);

// #endif // __UART_H__

#ifndef __UART_H__
#define __UART_H__

#include "config.h"
#include <string.h>
#include <stdio.h>

typedef enum
{
    DEBUG_STRING,
    DEBUG_WAVE,
    CUSTOM,
} UART_MODE;

typedef struct Class_UART
{
    uint8_t index;  // 串口索引号
    UART_MODE mode; // 串口模式

    uint8_t current_byte; // 当前接收的字节
    uint8_t rx_max_len;   // 接收数据最大长度
    uint8_t rx_len;       // 接收数据长度
    uint8_t *rxbuffer;    // 接收数据缓冲区

    bool is_inited;     // 串口初始化标志位
    bool is_configured; // 串口配置标志位

    uint8_t param_len;  // 参数个数
    float **param_list; // 参数列表

    void (*Init)(struct Class_UART *this, uint8_t rx_max_len, uint8_t param_len);    // 初始化函数
    void (*Send_Bit)(struct Class_UART *this, uint8_t data);                         // 发送数据函数
    void (*Send_Datas)(struct Class_UART *this, uint8_t *datas, size_t size);        // 发送字符串函数
    void (*Clear_RxBuffer)(struct Class_UART *this);                                 // 清除接收缓冲区
    
    void (*Custom_Send_Datas)(uint8_t *datas, size_t size); // 自定义发送数据函数

    void (*Send)(struct Class_UART *this, uint8_t *datas, size_t size); // 发送数据函数

    void (*Configure_Mode)(struct Class_UART *this, UART_MODE mode);                                // 配置调试模式
    void (*Configure_Param_Len)(struct Class_UART *this, uint8_t param_len);                        // 配置参数长度
    void (*Configure_Callback)(struct Class_UART *this, void (*callback)(struct Class_UART *this)); // 配置回调函数

    void (*Bind_Param_With_Id)(struct Class_UART *this, uint8_t id, float *input_param); // 绑定参数
    void (*Modify_Param_With_Id)(struct Class_UART *this, uint8_t id, float value);      // 修改参数

    void (*UART_INST_DataProcess)(struct Class_UART *this); // 串口中断处理函数

} Class_UART, *pClass_UART;

pClass_UART Create_UART(uint8_t index);   // 创建UART对象实例
pClass_UART Get_UART_INST(uint8_t index); // 获取UART对象实例

void UART_Init(pClass_UART this, uint8_t rx_max_len, uint8_t param_len);

void UART_Configure_Mode(pClass_UART this, UART_MODE mode);
void UART_Configure_Param_Len(pClass_UART this, uint8_t param_len);
void UART_Configure_Custom_Send_Datas(pClass_UART this, void (*func)(uint8_t *datas, size_t size));
void UART_Configure_Callback(pClass_UART this, void (*callback)(pClass_UART this));
void UART_Send_Bit(pClass_UART this, uint8_t data);
void UART_Send_Datas(pClass_UART this, uint8_t *datas, size_t size);
void UART_Clear_RxBuffer(pClass_UART this);
void UART_Bind_Param_With_Id(pClass_UART this, uint8_t id, float *input_param);
void UART_Modify_Param_With_Id(pClass_UART this, uint8_t id, float value);

void UART_Send(pClass_UART this, uint8_t *datas, size_t size);

IRQn_Type Get_UART_IRQN_From_Index(uint8_t index);
UART_Regs *Get_UART_INST_From_Index(uint8_t index);

#endif // __UART_H__
