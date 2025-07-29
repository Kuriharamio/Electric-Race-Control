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

#ifdef USE_PTZ
// 上舵机向上
void Servo_UP_UP(void)
{
  pClass_FT_Servo servo = Get_PTZ_INST()->Servo_Up;
  if (servo->is_inited)
  {
    if(servo->Error != -5){
      servo->Set_Target_Status(servo, --servo->Target_Pos, 20, 20);
    }
  }
}

// 上舵机向下
void Servo_UP_DOWN(void)
{
  pClass_FT_Servo servo = Get_PTZ_INST()->Servo_Up;
  if (servo->is_inited)
  {
    servo->Set_Target_Status(servo, ++servo->Target_Pos, 20, 20);
  }
}

// 下舵机向左
void Servo_DOWN_LEFT(void)
{
  pClass_FT_Servo servo = Get_PTZ_INST()->Servo_Down;
  if (servo->is_inited)
  {
    servo->Set_Target_Status(servo, --servo->Target_Pos, 20, 20);
  }
}

// 下舵机向右
void Servo_DOWN_RIGHT(void)
{
  pClass_FT_Servo servo = Get_PTZ_INST()->Servo_Down;
  if (servo->is_inited)
  {
    servo->Set_Target_Status(servo, ++servo->Target_Pos, 20, 20);
  }
}

void Toggle_LAZER(void)
{
  LAZER(TOGGLE);
}

void Set_Point_1(void)
{
  pClass_PTZ PTZ = Get_PTZ_INST();
  PTZ->Servo_Down->Set_Min_Pos(PTZ->Servo_Down, PTZ->Servo_Down->Pos);
  PTZ->Servo_Up->Set_Min_Pos(PTZ->Servo_Up, PTZ->Servo_Up->Pos);
  LED(TOGGLE);
}

void Set_Point_2(void)
{
  pClass_PTZ PTZ = Get_PTZ_INST();
  PTZ->Servo_Down->Set_Max_Pos(PTZ->Servo_Down, PTZ->Servo_Down->Pos);
  PTZ->Servo_Up->Set_Max_Pos(PTZ->Servo_Up, PTZ->Servo_Up->Pos);
  LED(TOGGLE);
}

bool ptz_task_1_flag = false;
void PTZ_Task_1(void)
{
  pClass_PTZ PTZ = Get_PTZ_INST();
  if (PTZ->is_inited)
  {
    ptz_task_1_flag = true;
  }
}

bool ptz_task_2_flag = false;
void PTZ_Task_2(void)
{
  pClass_PTZ PTZ = Get_PTZ_INST();
  if (PTZ->is_inited)
  {
    ptz_task_2_flag = true;
  }
}

bool ptz_task_3_flag = false;
void PTZ_Task_3(void)
{
  pClass_PTZ PTZ = Get_PTZ_INST();
  if (PTZ->is_inited)
  {
    ptz_task_3_flag = true;
  }
}

void EnablePID(void)
{
  pClass_PTZ PTZ = Get_PTZ_INST();
  if (PTZ->is_inited)
  {
    if(PTZ->PID_Enable)
    PTZ->PID_Enable = false;
    else
    PTZ->PID_Enable = true;
  }
}



#endif

int main(void)
{
  SYSCFG_DL_init();
  Enable_All_Interrupt();

  delay_ms(1000);
#ifdef USE_CAR
  pClass_Car Car = Create_Car(); // 创建小车对象
  Car->Init(Car);                // 初始化小车对象
#endif

//* 云台配置
#ifdef USE_PTZ
  pClass_PTZ PTZ = Create_PTZ();
  PTZ->Init(PTZ);
  delay_ms(1000);
  PTZ->Servo_Up->Set_Target_Status(PTZ->Servo_Up, 0.5 * (PTZ->Servo_Up->Max_Pos + PTZ->Servo_Up->Min_Pos), 30, 0);
  PTZ->Servo_Down->Set_Target_Status(PTZ->Servo_Down, 0.5 * (PTZ->Servo_Down->Max_Pos + PTZ->Servo_Down->Min_Pos), 30, 0);
#endif

//* 蓝牙配置
#ifdef USE_BLUETOOTH
  float a = 0;
  float b = 0;
  float c = 0;
  pClass_UART Bluetooth_Debuger = Create_UART(BLUETOOTH_UART_INDEX);               // 获取蓝牙对象实例
  Bluetooth_Debuger->Init(Bluetooth_Debuger, BLUETOOTH_RX_LEN_MAX, 5);             // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, DEBUG_WAVE);                // 配置调试模式
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_Rx_Callback); // 配置回调函数
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(PTZ->Servo_Down->Error), " ");
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(PTZ->Servo_Up->Error), " ");
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &(a), " ");
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 3, &(b), " ");
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 4, &(c), " ");
#endif

//* HMI配置
#ifdef USE_HMI
  pClass_UART HMI = Create_UART(HMI_UART_INDEX);
  HMI->Init(HMI, HMI_RX_LEN_MAX, 2);
  HMI->Configure_Mode(HMI, HMI_WATCH);
  HMI->Configure_Callback(HMI, HMI_Rx_Callback);
  HMI->Bind_Param_With_Id(HMI, 0, &(Car->gray_scale_sensor->Follow_Error), "Follow_Error");
  HMI->Bind_Param_With_Id(HMI, 1, &(Car->IMU_Yaw), "IMU_Yaw");
#endif

//* IMU配置
#ifdef USE_IMU
  pClass_UART IMU_Communicator = Create_UART(IMU_UART_INDEX);                      // 获取IMU串口对象实例
  IMU_Communicator->Init(IMU_Communicator, IMU_RX_LEN_MAX, 1);                     // 初始化IMU串口对象
  IMU_Communicator->Configure_Mode(IMU_Communicator, RAW_DATA);                // 配置调试模式
  IMU_Communicator->Configure_Callback(IMU_Communicator, IMU_Rx_Callback);         // 配置IMU回调函数
  IMU_Communicator->Bind_Param_With_Id(IMU_Communicator, 0, &(Car->IMU_Yaw), " "); // 绑定参数
  IMU_Init();                                                                      // 校準加速度
#endif


//* Raspberry串口通信配置
#ifdef USE_RASPBERRY
  pClass_UART Raspberry_Communicator = Create_UART(Raspberry_UART_INDEX);                            // 获取Raspberry串口对象实例
  Raspberry_Communicator->Init(Raspberry_Communicator, Raspberry_RX_LEN_MAX, 2);                         // 初始化Raspberry串口对象
  Raspberry_Communicator->Configure_Mode(Raspberry_Communicator, CUSTOM);                            // 配置调试模式
  Raspberry_Communicator->Configure_Callback(Raspberry_Communicator, Raspberry_Rx_Callback);             // 配置回调函数
  Raspberry_Communicator->Configure_Send_Float(Raspberry_Communicator, Raspberry_Transmit);
  Raspberry_Communicator->Bind_Param_With_Id(Raspberry_Communicator, 0, &(PTZ->Servo_Down->Error), " "); // 绑定参数0
  Raspberry_Communicator->Bind_Param_With_Id(Raspberry_Communicator, 1, &(PTZ->Servo_Up->Error), " ");
#endif

//* 按键配置
#ifdef USE_ADC_BUTTON
  pClass_ADCButton ADC_Button = Create_ADCButton();                                                   // 创建按键对象
  ADC_Button->Init(ADC_Button, ADC_BUTTON_INST, ADC_BUTTON_INST_INT_IRQN, ADC_BUTTON_ADCMEM_ADC_CH0); // 初始化按键对象
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_1, EnablePID, Toggle_LAZER, NULL);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_2, Set_Point_1, NULL, Servo_UP_UP);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_3, PTZ_Task_1, NULL, Servo_DOWN_LEFT);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_4, PTZ_Task_2, NULL, Servo_DOWN_RIGHT);
  ADC_Button->Configure_Callback(ADC_Button, BUTTON_5, Set_Point_2, NULL, Servo_UP_DOWN);
#endif
  LAZER(ON);
  while (1)
  {
    a = (float)(PTZ->Servo_Down->Output_Value);
    b = (float)(PTZ->Servo_Up->Output_Value);
    c = (float)(PTZ->Task_ID);
#ifdef USE_BLUETOOTH
    Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
#endif

#ifdef USE_HMI
    HMI->Send(HMI, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
#endif

#ifdef USE_RASPBERRY
    float datas[3] = {(float)(PTZ->Servo_Down->Pos), (float)(PTZ->Servo_Up->Pos), (float)(PTZ->Task_ID)};
    if(Raspberry_Communicator->Send_Float){
      Raspberry_Communicator->Send_Float(Raspberry_Communicator, datas, sizeof(datas) / sizeof(float));
    }
#endif

    if (BUZZ_STATE == BEEP)
    {
      BUZZ(BEEP);
      BUZZ_STATE = OFF;
    }

    // {
    //   if(ptz_task_1_flag){
    //     if(PTZ->is_inited){
    //       PTZ->Draw_Img(PTZ);
    //       ptz_task_1_flag = false;
    //     }
    //   }
    //   if (ptz_task_2_flag)
    //   {
    //     if (PTZ->is_inited)
    //     {
    //       PTZ->Draw_Func(PTZ);
    //       ptz_task_2_flag = false;
    //     }
    //   }
    //   if(ptz_task_3_flag)
    //   {
    //     if(PTZ->is_inited)
    //     {
    //       PTZ->Draw_Num(PTZ, 0.123456789, true, "%.10f");
    //       ptz_task_3_flag = false;
    //     }
    //   }
    // }

    delay_ms(10);
  }
}
