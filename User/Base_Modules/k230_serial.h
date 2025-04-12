#ifndef __K230_SERIAL_H__
#define __K230_SERIAL_H__

#include "BSP/uart.h"

#define FRAME_HEADER_1 0xA5
#define FRAME_HEADER_2 0x5A

// 接收数据状态机，用于判断当前数据接收到哪一步
typedef enum {
    STATE_WAIT_HEADER_1,
    STATE_WAIT_HEADER_2,
    STATE_WAIT_LENGTH,
    STATE_WAIT_DATA,
    STATE_WAIT_BCC
} RxState;

void float_to_big_endian_bytes(float value, uint8_t *bytes_out);
float BigEndianBytesToFloat(uint8_t *bytes);
uint8_t Calculate_BCC(uint8_t *data, uint8_t len);
void K230_Rx_Callback(pClass_UART this);
void K230_Transmit(pClass_UART this, float *datas, uint8_t len);

#endif // __K230_SERIAL_H__