#ifndef _IMU_H_
#define _IMU_H_

#include "config.h"
#include "BSP/uart.h"
#include "Algorithm/drv_math.h"

void IMU_Rx_Callback(pClass_UART this);

#endif // _IMU_H_