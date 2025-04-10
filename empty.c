#include "ti_msp_dl_config.h"

#include "BSP/board.h"
#include "BSP/delay.h"
#include "Base_Modules/led.h"
#include "Base_Modules/bluetooth.h"
#include "Base_Modules/motor.h"
#include <stdlib.h>

int main(void)
{
  board_init();

  // 电机配置(测试，之后移动到小车类中)
  pClass_Motor Motor_LF = create_motor(LEFT_FRONT);                                                                                                    // 创建电机对象
  Motor_LF->Init(Motor_LF, 0.024, 1600, 1.5, 20, 13, 4);                                                                                               // 初始化电机对象
  Motor_LF->PID_Speed->PID_Init(Motor_LF->PID_Speed, 2000, 10000.0, 2000.0, 5000.0, 1500, 1600, PID_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
  Motor_LF->Configure_IN_1(Motor_LF, MOTOR_DRV_LF_IN1_PORT, MOTOR_DRV_LF_IN1_PIN);                                                                     // 配置电机引脚IN1
  Motor_LF->Configure_IN_2(Motor_LF, MOTOR_DRV_LF_IN2_PORT, MOTOR_DRV_LF_IN2_PIN);                                                                     // 配置电机引脚IN2
  Motor_LF->Configure_ENCODER_A(Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_A_PIN);                                                                       // 配置电机引脚编码器A
  Motor_LF->Configure_ENCODER_B(Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_B_PIN);                                                                       // 配置电机引脚编码器B
  Motor_LF->Configure_PWM(Motor_LF, PWM_MOTOR_INST, GPIO_PWM_MOTOR_C0_IDX);
  Motor_LF->Configure_STBY(Motor_LF, MOTOR_DRV_STBY_F_PORT, MOTOR_DRV_STBY_F_PIN); // 配置电机待机引脚

  // 蓝牙配置
  pClass_Bluetooth Bluetooth_Debuger = Create_Bluetooth();                                      // 创建蓝牙对象
  Bluetooth_Debuger->Init(Bluetooth_Debuger, BLUETOOTH_UART_INDEX, 3);                          // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_0_Rx_Callback);            // 配置回调函数
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, WAVE);                                   // 配置调试模式
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &(Motor_LF->Now_Speed));          // 绑定参数0
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &(Motor_LF->Target_Speed));       // 绑定参数1
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 2, &(Motor_LF->Output_Now));         // 绑定参数2


  while (1)
  {
    Bluetooth_Debuger->Send_Datas(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据

    // 电机响应测试 >>>
    static int t = 0;
    // Motor_LF->Target_Speed = sin(0.01 * t);
    t++;
    if (t % 500 == 0)
    {
      Motor_LF->Target_Speed = 0.3f;
    }
    // <<< 电机响应测试

    delay_ms(10);
  }
}
