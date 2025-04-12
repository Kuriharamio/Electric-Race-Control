#include "ti_msp_dl_config.h"
#include <stdlib.h>

#include "BSP/board.h"
#include "BSP/delay.h"
#include "BSP/timer.h"

#include "Base_Modules/led.h"
#include "Base_Modules/bluetooth.h"
#include "Base_Modules/k230_serial.h"
#include "Base_Modules/motor.h"
#include "Base_Modules/adc_button.h"
#include "Base_Modules/buzz.h"

#include "Car/car.h"

void Test_Button_Event(void)
{
  pClass_UART Bluetooth_Debuger = Get_UART_INST(0);   // 获取蓝牙对象实例
  pClass_ADCButton Adc_Button = GET_ADCButton_INST(); // 获取按键对象实例

  char str[50];
  sprintf(str, "Button %d Pressed with ADC value: %d \r\n", Adc_Button->Current_Button, Adc_Button->Current_ADC_Value); // 格式化字符串
  Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)str, sizeof(str));                                              // 发送数据
}

int main(void)
{
  board_init();
  float x = 1.0f;
  float y = 0.1f;
  float	z = 0.0f;
	
  pClass_Car Car = Create_Car(); // 创建小车对象
  Car->Init(Car);                // 初始化小车对象

  // K230串口通信配置
  pClass_UART K230_Communicator = Create_UART(1);                             // 获取K230串口对象实例
  K230_Communicator->Init(K230_Communicator, K230_RX_LEN_MAX, 3);             // 初始化K230串口对象
  K230_Communicator->Configure_Mode(K230_Communicator, CUSTOM);           // 配置调试模式
  K230_Communicator->Configure_Param_Len(K230_Communicator, 3);
  K230_Communicator->Configure_Callback(K230_Communicator, K230_Rx_Callback); // 配置回调函数
  K230_Communicator->Bind_Param_With_Id(K230_Communicator, 0, &x);           // 绑定参数0
  K230_Communicator->Bind_Param_With_Id(K230_Communicator, 1, &y);           // 绑定参数1
  K230_Communicator->Bind_Param_With_Id(K230_Communicator, 2, &z);           // 绑定参数2
  // 按键配置
  pClass_ADCButton Adc_Button = Create_AdcButton();                                   // 创建按键对象
  Adc_Button->Init(Adc_Button, adckey_INST, adckey_INST_INT_IRQN, adckey_ADCMEM_key); // 初始化按键对象
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_1, Test_Button_Event);            // 配置按键1的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_2, Test_Button_Event);            // 配置按键2的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_3, Test_Button_Event);            // 配置按键3的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_4, Test_Button_Event);            // 配置按键4的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_5, Test_Button_Event);            // 配置按键5的回调函数

  // 蓝牙配置
  pClass_UART Bluetooth_Debuger = Create_UART(0);                                                // 获取蓝牙对象实例
  Bluetooth_Debuger->Init(Bluetooth_Debuger, BLUETOOTH_RX_LEN_MAX, 3);                           // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, DEBUG_WAVE);                              // 配置调试模式
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_Rx_Callback);   // 配置回调函数
  Bluetooth_Debuger->Configure_Param_Len(Bluetooth_Debuger, 3);
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &x);           // 绑定参数0
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &y);           // 绑定参数1
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &z);           // 绑定参数2
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Car->Motor_LB->Target_Speed)); // 绑定参数0
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(Car->Target_Position.x)); // 绑定参数1
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &(Car->Now_Position.x)); // 绑定参数2
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 3, &(Adc_Button->Current_ADC_Value)); // 绑定参数3

  Car->Target_Position.x = 1.0f; // 设置目标位置x坐标

  while (1)
  {
    // Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
	  Bluetooth_Debuger->Send(Bluetooth_Debuger,NULL,NULL);
    // 电机响应测试 >>>
    static int t = 0;
    // Car->Motor_LB->Target_Speed = sin(0.01 * t);
    // Car->Motor_LF->Target_Speed = sin(0.01 * t);
    // Car->Motor_RB->Target_Speed = sin(0.01 * t);
    // Car->Motor_RF->Target_Speed = sin(0.01 * t);
    // Car->Target_Speed.linear_velocity = sin(0.01 * t); // 设置目标线速度
    // Car->Target_Position.x = sin(0.01 * t); // 设置目标位置x坐标
    t++;
    // if (t % 500 == 0)
    // {
    // Car->Motor_RF->Target_Speed = 0.3f;
    // }
    // <<< 电机响应测试
	delay_ms(10);
  }
}
