#include "User/config.h"

#include "BSP/board.h"
#include "BSP/delay.h"
#include "BSP/timer.h"

#include "Base_Modules/reminder.h"
#include "Base_Modules/bluetooth.h"
#include "Base_Modules/k230_serial.h"
#include "Base_Modules/imu.h"
#include "Base_Modules/motor.h"
#include "Base_Modules/adc_button.h"
#include "Base_Modules/reminder.h"

#include "Car/car.h"

#include "problem.h"


int main(void)
{
  SYSCFG_DL_init();

  pClass_Car Car = Create_Car(); // 创建小车对象
  Car->Init(Car);                // 初始化小车对象

  // 蓝牙配置
  pClass_UART Bluetooth_Debuger = Create_UART(BLUETOOTH_UART_INDEX);                                                 // 获取蓝牙对象实例
  Bluetooth_Debuger->Init(Bluetooth_Debuger, BLUETOOTH_RX_LEN_MAX, 1);                            // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, DEBUG_STRING);                               // 配置调试模式
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_Rx_Callback);                // 配置回调函数
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Car->gray_scale_sensor->Follow_Error)); // 绑定参数0

  // 按键配置
  // pClass_ADCButton Adc_Button = Create_AdcButton();                                   // 创建按键对象
  // Adc_Button->Init(Adc_Button, adckey_INST, adckey_INST_INT_IRQN, adckey_ADCMEM_key); // 初始化按键对象
  // Adc_Button->Configure_Callback(Adc_Button, BUTTON_1, Change_Mode_0);                // 配置按键1的回调函数
  // Adc_Button->Configure_Callback(Adc_Button, BUTTON_2, Change_Mode_1);                // 配置按键2的回调函数
  // Adc_Button->Configure_Callback(Adc_Button, BUTTON_3, Test_Button_Event);            // 配置按键3的回调函数
  // Adc_Button->Configure_Callback(Adc_Button, BUTTON_4, Test_Button_Event);            // 配置按键4的回调函数
  // Adc_Button->Configure_Callback(Adc_Button, BUTTON_5, Test_Button_Event);            // 配置按键5的回调函数

  // IMU配置
  // pClass_UART IMU_Communicator = Create_UART(IMU_UART_INDEX);                             // 获取IMU串口对象实例
  // IMU_Communicator->Init(IMU_Communicator, IMU_RX_LEN_MAX, 1);               // 初始化IMU串口对象
  // IMU_Communicator->Configure_Mode(IMU_Communicator, DEBUG_STRING);
  // IMU_Communicator->Configure_Callback(IMU_Communicator, IMU_Rx_Callback); // 配置IMU回调函数
  // IMU_Communicator->Bind_Param_With_Id(IMU_Communicator, 0, &(Car->IMU_Yaw)); // 绑定参数0

  // K230串口通信配置
  // pClass_UART K230_Communicator = Create_UART(K230_UART_INDEX);   // 获取K230串口对象实例
  // K230_Communicator->Init(K230_Communicator, K230_RX_LEN_MAX, 1); // 初始化K230串口对象
  // K230_Communicator->Configure_Mode(K230_Communicator, DEBUG_WAVE);   // 配置调试模式
  // K230_Communicator->Configure_Callback(K230_Communicator, K230_Rx_Callback); // 配置回调函数
  // K230_Communicator->Bind_Param_With_Id(K230_Communicator, 0, &(Car->follow_error));            // 绑定参数0

  // pClass_UART FT_Servo_Controller = Create_UART(FT_SERVO_UART_INDEX);
  // FT_Servo_Controller->Init(FT_Servo_Controller, FT_SERVO_RX_LEN_MAX, 1);
  // FT_Servo_Controller->Configure_Mode(FT_Servo_Controller, DEBUG_STRING);
  // FT_Servo_Controller->Configure_Callback(FT_Servo_Controller, FT_Servo_Data_Process);

  // pClass_FT_Servo servo_1 = Create_FT_Servo(1);
  // pClass_FT_Servo servo_2 = Create_FT_Servo(2);

  // servo_1->Init(servo_1, POS_MODE_ABSOLUTE, 0, 4095, 0, 60, 0, 50);
  // servo_2->Init(servo_2, POS_MODE_ABSOLUTE, 0, 4095, 0, 60, 0, 50);

  Enable_All_Interrupt();
  while (1)
  {
    LED(TOGGLE);
    Car->gray_scale_sensor->Update(Car->gray_scale_sensor);
    // // Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
    printf("Follow Error: %.2f\r\n", Car->gray_scale_sensor->Follow_Error); // 打印循迹误差
    // {
    //     static int count = 0;
    //     count++;
    //     if(count == 3*100){
    //       Car->Task_ID = 3;
    //     }
    // }

    delay_ms(1);
    }
}
