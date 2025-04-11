#ifndef __K230_SERIAL_H__
#define __K230_SERIAL_H__

#include "BSP/uart.h"

#define FRAME_HEADER 0xAA
#define MAX_DATA_LEN 64   //最大数据接收长度，单位为字节

// 接收数据状态机，用于判断当前数据接收到哪一步
typedef enum {
    STATE_WAIT_HEADER,
    STATE_WAIT_LENGTH,
    STATE_WAIT_DATA,
    STATE_WAIT_BCC
} RxState;
	
uint8_t Calculate_BCC(uint8_t *data, uint8_t len);
void K230_Rx_Callback(pClass_UART this);

#endif // __K230_SERIAL_H__