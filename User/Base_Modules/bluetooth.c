#include "Base_Modules/bluetooth.h"
/**
 * @brief 蓝牙接收回调
 *
 * @param this 蓝牙对象
 */
void Bluetooth_Rx_Callback(pClass_UART this)
{

    if (this->current_byte != '?')
    {
        if (this->rx_len < BLUETOOTH_RX_LEN_MAX)
        {
            this->rxbuffer[this->rx_len++] = this->current_byte; // 接收数据
        }
        else
        {
            this->rx_len = 0; // 清空接收数据
        }
    }
    else
    {
        this->rxbuffer[this->rx_len++] = '\0';
        int id = 0;
        float value = 0.0f;
        sscanf((char *)(this->rxbuffer), "%d=%f", &id, &value);
        this->Modify_Param_With_Id(this, id, value); // 修改参数

       
    }
}

void Test_Button_Event(void)
{
    pClass_UART Bluetooth_Debuger = Get_UART_INST(0); // 获取蓝牙对象实例
    pClass_ADCButton Adc_Button = GET_ADCButton_INST(); // 获取按键对象实例

    char str[50];
    sprintf(str, "Button %d Pressed with ADC value: %d \r\n", Adc_Button->Current_Button, Adc_Button->Current_ADC_Value); // 格式化字符串
    Bluetooth_Debuger->Send(Bluetooth_Debuger, (uint8_t *)str, sizeof(str)); // 发送数据
}