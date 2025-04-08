#include "ti_msp_dl_config.h"

#include "BSP/board.h"
#include "BSP/delay.h"
#include "Base_Modules/led.h"
#include "Base_Modules/bluetooth.h"
#include "Base_Modules/motor.h"

void return_data(pClass_UART this)
{
  this->Send_Datas(this, this->rxbuffer, this->rx_len); // 回传数据
  this->Clear_Buffer(this);                             // 清空接收数据
}

int main(void)
{
  board_init();

  // 电机配置
  pClass_Motor Motor_RF = create_motor(RIGHT_BACK);     // 创建电机对象
  Motor_RF->Init(Motor_RF, 0.024, 1600, 1.5, 20, 13, 4);                              // 初始化电机对象
  Motor_RF->PID_Speed->PID_Init(Motor_RF->PID_Speed, 100, 10.0, 0.0, 0.0, 800, 1600, PID_TIMER_T, 0.0, 0.0, 0.0, 0.0, PID_D_First_DISABLE); // 初始化PID参数
  Motor_RF->Configure_IN_1(Motor_RF, MOTOR_GROUP_L_F_IN1_PORT, MOTOR_GROUP_L_F_IN1_PIN); // 配置电机引脚IN1
  Motor_RF->Configure_IN_2(Motor_RF, MOTOR_GROUP_L_F_IN2_PORT, MOTOR_GROUP_L_F_IN2_PIN); // 配置电机引脚IN2
  Motor_RF->Configure_ENCODER_A(Motor_RF, MOTOR_GROUP_L_F_A_PORT, MOTOR_GROUP_L_F_A_PIN); // 配置电机引脚编码器A
  Motor_RF->Configure_ENCODER_B(Motor_RF, MOTOR_GROUP_L_F_B_PORT, MOTOR_GROUP_L_F_B_PIN); // 配置电机引脚编码器B
  Motor_RF->Configure_PWM(Motor_RF, PWM_MOTOR_INST, GPIO_PWM_MOTOR_C0_IDX);
  Motor_RF->Target_Speed = 0.5f;

  // 蓝牙配置
  // pClass_Bluetooth Bluetooth_Debuger = Create_Bluetooth();                           // 创建蓝牙对象
  // Bluetooth_Debuger->Init(Bluetooth_Debuger, 0, 2);                                  // 初始化蓝牙对象
  // Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_0_Rx_Callback); // 配置回调函数
  // Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, WAVE);                        // 配置调试模式
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Motor_RF->Now_Speed));             // 绑定参数
  // Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(Motor_RF->Target_Speed));             // 绑定参数

  // DL_GPIO_setPins(MOTOR_GROUP_STBY_PORT, MOTOR_GROUP_STBY_PIN); // 设置电机待机引脚
  // DL_GPIO_setPins(MOTOR_GROUP_L_F_IN1_PORT, MOTOR_GROUP_L_F_IN1_PIN); // 设置电机引脚IN1
  // DL_GPIO_clearPins(MOTOR_GROUP_L_F_IN2_PORT, MOTOR_GROUP_L_F_IN2_PIN); // 设置电机引脚IN2
  // DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST, 500, GPIO_PWM_MOTOR_C0_IDX);

  while (1)
  {
    // Bluetooth_Debuger->Send_Datas(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
    LED(TOGGLE);                                                                         // 切换LED灯状态
    delay_ms(1000);                                                                      // 延时1秒
  }
}
