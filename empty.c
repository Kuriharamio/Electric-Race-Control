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

  // 电机配置(测试，之后移动到小车类中)
  // pClass_Motor Motor_LF = create_motor(LEFT_FRONT);                                                                                                    // 创建电机对象
  // Motor_LF->Init(Motor_LF, 0.024, 1600, 1.5, 20, 13, 4);                                                                                               // 初始化电机对象
  // Motor_LF->PID_Speed->PID_Init(Motor_LF->PID_Speed, 2000, 10000.0, 2000.0, 5000.0, 1500, 1600, PID_MOTOR_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
  // Motor_LF->Configure_IN_1(Motor_LF, MOTOR_DRV_LF_IN1_PORT, MOTOR_DRV_LF_IN1_PIN);                                                                     // 配置电机引脚IN1
  // Motor_LF->Configure_IN_2(Motor_LF, MOTOR_DRV_LF_IN2_PORT, MOTOR_DRV_LF_IN2_PIN);                                                                     // 配置电机引脚IN2
  // Motor_LF->Configure_ENCODER_A(Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_A_PIN);                                                                       // 配置电机引脚编码器A
  // Motor_LF->Configure_ENCODER_B(Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_B_PIN);                                                                       // 配置电机引脚编码器B
  // Motor_LF->Configure_PWM(Motor_LF, PWM_MOTOR_INST, GPIO_PWM_MOTOR_C0_IDX);
  // Motor_LF->Configure_STBY(Motor_LF, MOTOR_DRV_STBY_F_PORT, MOTOR_DRV_STBY_F_PIN); // 配置电机待机引脚

  pClass_Car Car = Create_Car(); // 创建小车对象
  Car->Init(Car);               // 初始化小车对象

  // 蓝牙配置
  pClass_UART Bluetooth_Debuger = Create_UART(0);                                         // 获取蓝牙对象实例
  Bluetooth_Debuger->Init(Bluetooth_Debuger, BLUETOOTH_RX_LEN_MAX, 3);                    // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, DEBUG_WAVE);                       // 配置调试模式
  Bluetooth_Debuger->Configure_Param_Len(Bluetooth_Debuger, 3);                           // 配置参数长度
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_Rx_Callback);        // 配置回调函数
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Car->Motor_LF->Now_Speed));    // 绑定参数0
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(Car->Motor_LF->Target_Speed)); // 绑定参数1
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &(Car->Motor_LF->Output_Now));   // 绑定参数2

  // K230串口通信配置
  pClass_UART K230_Communicator = Create_UART(1);                             // 获取蓝牙对象实例
  K230_Communicator->Init(K230_Communicator, K230_RX_LEN_MAX, 3);        // 初始化蓝牙对象
  K230_Communicator->Configure_Callback(K230_Communicator, K230_Rx_Callback); // 配置回调函数
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Motor_LF->Now_Speed));    // 绑定参数0
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(Motor_LF->Target_Speed)); // 绑定参数1
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &(Motor_LF->Output_Now));   // 绑定参数2

  // 按键配置
  pClass_ADCButton Adc_Button = Create_AdcButton();                                   // 创建按键对象
  Adc_Button->Init(Adc_Button, adckey_INST, adckey_INST_INT_IRQN, adckey_ADCMEM_key); // 初始化按键对象
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_1, Test_Button_Event);            // 配置按键1的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_2, Test_Button_Event);            // 配置按键2的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_3, Test_Button_Event);            // 配置按键3的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_4, Test_Button_Event);            // 配置按键4的回调函数
  Adc_Button->Configure_Callback(Adc_Button, BUTTON_5, Test_Button_Event);            // 配置按键5的回调函数
  while (1)
  {
    Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据

    // 电机响应测试 >>>
    static int t = 0;
    Car->Target_Speed.linear_velocity = sin(0.01 * t);
    t++;
    // if (t % 500 == 0)
    // {
    //   Motor_LF->Target_Speed = 0.3f;
    // }
    // <<< 电机响应测试

    delay_ms(10);
  }
}
