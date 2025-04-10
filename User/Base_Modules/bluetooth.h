#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "BSP/uart.h"
void Bluetooth_Rx_Callback(pClass_UART this);

#include "Base_Modules/adc_button.h"
void Test_Button_Event(void);

#endif // __BLUETOOTH_H__