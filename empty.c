#include "User/config.h"

#include "BSP/board.h"
#include "BSP/timer.h"

#include "Base_Modules/reminder.h"
#include "Base_Modules/bluetooth.h"
#include "Base_Modules/Raspberry_serial.h"
#include "Base_Modules/imu.h"
#include "Base_Modules/motor.h"
#include "Base_Modules/adc_button.h"
#include "Base_Modules/reminder.h"
#include "Base_Modules/hmi.h"
#include "Base_Modules/ptz.h"

#include "Car/car.h"


int main(void)
{
  SYSCFG_DL_init();

#ifdef USE_CAR
  pClass_Car Car = Create_Car(); // 创建小车对象
  Car->Init(Car);                // 初始化小车对象
#endif

//* 蓝牙配置
#ifdef USE_BLUETOOTH
  pClass_UART Bluetooth_Debuger = Create_UART(BLUETOOTH_UART_INDEX);               // 获取蓝牙对象实例
  Bluetooth_Debuger->Init(Bluetooth_Debuger, BLUETOOTH_RX_LEN_MAX, 3);             // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, DEBUG_WAVE);                // 配置调试模式
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_Rx_Callback); // 配置回调函数
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Car->Target_Speed.angular_velocity), "0");
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(Car->Target_Position.yaw), "1");
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &(Car->Now_Position.yaw), "2");
  
#endif

//* HMI配置
#ifdef USE_HMI
  float a = 0;
  float b = 0;
  float c = 0;
  // float d = 0;
  pClass_UART HMI = Create_UART(HMI_UART_INDEX);
  HMI->Init(HMI, HMI_RX_LEN_MAX, 4);
  HMI->Configure_Mode(HMI, HMI_WATCH);
  HMI->Configure_Callback(HMI, HMI_Rx_Callback);
  HMI->Bind_Param_With_Id(HMI, 0, &(Car->gray_scale_sensor->Follow_Error), "F_E");
  HMI->Bind_Param_With_Id(HMI, 1, &(a), "ID");
  HMI->Bind_Param_With_Id(HMI, 2, &(b), "cnt");
  HMI->Bind_Param_With_Id(HMI, 3, &(c), "MODE");
#endif

//* IMU配置
#ifdef USE_IMU
  pClass_UART IMU_Communicator = Create_UART(IMU_UART_INDEX);                      // 获取IMU串口对象实例
  IMU_Communicator->Init(IMU_Communicator, IMU_RX_LEN_MAX, 1);                     // 初始化IMU串口对象
  IMU_Communicator->Configure_Mode(IMU_Communicator, DEBUG_STRING);                // 配置调试模式
  IMU_Communicator->Configure_Callback(IMU_Communicator, IMU_Rx_Callback);         // 配置IMU回调函数
  IMU_Communicator->Bind_Param_With_Id(IMU_Communicator, 0, &(Car->IMU_Yaw), " "); // 绑定参数
  IMU_Init();                                                                      // 校準加速度
#endif

//* 云台配置
#ifdef USE_PTZ
  delay_ms(1000);
  pClass_PTZ PTZ = Create_PTZ();
  PTZ->Init(PTZ);
  PTZ->Servo_Up->Set_Target_Status(PTZ->Servo_Up, 2047, 30, 0);
  PTZ->Servo_Down->Set_Target_Status(PTZ->Servo_Down, 2047, 30, 0);
#endif

//* Raspberry串口通信配置
#ifdef USE_RASPBERRY
  pClass_UART Raspberry_Communicator = Create_UART(Raspberry_UART_INDEX);                                // 获取Raspberry串口对象实例
  Raspberry_Communicator->Init(Raspberry_Communicator, Raspberry_RX_LEN_MAX, 2);                         // 初始化Raspberry串口对象
  Raspberry_Communicator->Configure_Mode(Raspberry_Communicator, DEBUG_STRING);                            // 配置调试模式
  Raspberry_Communicator->Configure_Callback(Raspberry_Communicator, Raspberry_Rx_Callback);             // 配置回调函数
  Raspberry_Communicator->Bind_Param_With_Id(Raspberry_Communicator, 0, &(PTZ->Servo_Down->Error), " "); // 绑定参数0
  Raspberry_Communicator->Bind_Param_With_Id(Raspberry_Communicator, 1, &(PTZ->Servo_Up->Error), " ");
#endif

//* 按键配置
#ifdef USE_ADC_BUTTON
  pClass_ADCButton ADC_Button = Create_ADCButton();                                                   // 创建按键对象
  ADC_Button->Init(ADC_Button, ADC_BUTTON_INST, ADC_BUTTON_INST_INT_IRQN, ADC_BUTTON_ADCMEM_ADC_CH0); // 初始化按键对象
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_1, NULL, Toggle_LAZER, NULL);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_2, Set_Point_1, NULL, Servo_UP_UP);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_3, PTZ_Task_1, NULL, Servo_DOWN_LEFT);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_4, PTZ_Task_2, NULL, Servo_DOWN_RIGHT);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_5, Set_Point_2, NULL, Servo_UP_DOWN);
#endif
  delay_ms(4000);
  Enable_All_Interrupt();
  LED_STATE = BEEP;
  // Car->Mode = MOTOR_TEST;
  // Car->Target_Speed.linear_velocity = 1.0f;
  while (1)
  {
#ifdef USE_BLUETOOTH
    Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
#endif
#ifdef USE_HMI
    a = (float)(Car->Task_ID);
    b = (float)(Car->Task_1_cnt);
    c = (float)(Car->Mode);
    HMI->Send(HMI, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
#endif

    if (LED_STATE == BEEP)
    {
      // LED(BEEP);
      LED_STATE = OFF;
    }


    delay_ms(100);
  }
}
