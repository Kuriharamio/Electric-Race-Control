#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <string.h>
#include <stdio.h>
#include "BSP/uart.h"

typedef enum
{
    STRING,
    WAVE,
    CUSTOM,
} DEBUG_MODE;

typedef struct Class_Bluetooth
{
    pClass_UART UART_INST; // 串口实例

    DEBUG_MODE mode; // 调试模式
    bool is_inited;  // 初始化标志位

    uint8_t rxbuffer[20]; // 接收数据缓冲区

    uint8_t param_len;
    float **param_list;

    void (*Init)(struct Class_Bluetooth *this, uint8_t uart_index, uint8_t param_len);            // 初始化函数
    void (*Configure_Mode)(struct Class_Bluetooth *this, DEBUG_MODE mode);                        // 配置调试模式
    void (*Configure_Param_Len)(struct Class_Bluetooth *this, uint8_t param_len);                 // 配置参数长度
    void (*Configure_Callback)(struct Class_Bluetooth *this, void (*callback)(pClass_UART this)); // 配置回调函数
    void (*Send_Datas)(struct Class_Bluetooth *this, uint8_t *datas, size_t size);                // 发送数据函数
    void (*Bind_Param_With_Id)(struct Class_Bluetooth *this, uint8_t id, float *input_param);     // 绑定参数
    void (*Modify_Param_With_Id)(struct Class_Bluetooth *this, uint8_t id, float value);          // 修改参数

} Class_Bluetooth, *pClass_Bluetooth;

pClass_Bluetooth Create_Bluetooth(void);
void Bluetooth_Init(pClass_Bluetooth this, uint8_t uart_index, uint8_t param_len);
void Bluetooth_Configure_Mode(pClass_Bluetooth this, DEBUG_MODE mode);
void Bluetooth_Configure_Param_Len(pClass_Bluetooth this, uint8_t param_len);
void Bluetooth_Configure_Callback(pClass_Bluetooth this, void (*callback)(pClass_UART this));
void Bluetooth_Send_Datas(pClass_Bluetooth this, uint8_t *datas, size_t size);
void Bluetooth_Bind_Param_With_Id(pClass_Bluetooth this, uint8_t id, float *input_param);
void Bluetooth_Modify_Param_With_Id(pClass_Bluetooth this, uint8_t id, float value);

void Bluetooth_0_Rx_Callback(pClass_UART this);

#endif
