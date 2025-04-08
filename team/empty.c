#include "ti_msp_dl_config.h"

#include "BSP/board.h"
#include "BSP/delay.h"
#include "Base_Modules/led.h"
#include "Base_Modules/bluetooth.h"

void return_data(pClass_UART this)
{
  this->Send_Datas(this, this->rxbuffer, this->rx_len); // 回传数据
  this->Clear_Buffer(this);                             // 清空接收数据
}

int main(void)
{
  board_init();

  float param_0 = 0.0f; // 浮点数参数
  float param_1 = 0.0f; // 浮点数参数
  uint8_t TAIL[4] = {0x00, 0x00, 0x80, 0x7f}; // WAVE数据尾(JustFloat)

  // 蓝牙配置
  pClass_Bluetooth Bluetooth_Debuger = Create_Bluetooth();                           // 创建蓝牙对象
  Bluetooth_Debuger->Init(Bluetooth_Debuger, 0, 2);                                  // 初始化蓝牙对象
  Bluetooth_Debuger->Configure_Callback(Bluetooth_Debuger, Bluetooth_0_Rx_Callback); // 配置回调函数
  Bluetooth_Debuger->Configure_Mode(Bluetooth_Debuger, WAVE);                        // 配置调试模式
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 0, &param_0);             // 绑定参数
  Bluetooth_Debuger->Bind_Param_With_Id(Bluetooth_Debuger, 1, &param_1);             // 绑定参数

  while (1)
  {
//    Bluetooth_Debuger->Send_Datas(Bluetooth_Debuger, (uint8_t *)"Debugging...\r\n", 15); // 发送数据
//    LED(TOGGLE);                                                                         // 切换LED灯状态
//    delay_ms(1000);   	  // 延时1秒
	  Bluetooth_Debuger->Send_Datas(Bluetooth_Debuger,(uint8_t *)&param_0,sizeof(float));
	  Bluetooth_Debuger->Send_Datas(Bluetooth_Debuger,(uint8_t *)&param_1,sizeof(float));
	  Bluetooth_Debuger->Send_Datas(Bluetooth_Debuger,(uint8_t *)TAIL,4*sizeof(float));
	  Bluetooth_Debuger->UART_INST->UART_INST_DataProcess(Bluetooth_Debuger->UART_INST);
  }
}
