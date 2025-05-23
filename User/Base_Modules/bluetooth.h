#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "config.h"
#include "BSP/uart.h"
#include "BSP/board.h"

void Bluetooth_Rx_Callback(pClass_UART this);

#endif // __BLUETOOTH_H__