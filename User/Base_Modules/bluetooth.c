#include "Base_Modules/bluetooth.h"
/**
 * @brief 蓝牙接收回调
 *
 * @param this 串口对象
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

