/**
 * @file bt_debug.c
 * @author KuriharaMio & Zhangxin
 * @brief
 * @version 0.1
 * @date 2025-04-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Base_Modules/bluetooth.h"

uint8_t WAVE_TAIL[4] = {0x00, 0x00, 0x80, 0x7f}; // WAVE数据尾(JustFloat)

static Class_Bluetooth _Bluetooth_0 = {0}; // 静态实例化蓝牙对象

/**
 * @brief 创建蓝牙对象
 *
 * @return pClass_Bluetooth  返回蓝牙对象指针
 */
pClass_Bluetooth Create_Bluetooth(void)
{
    pClass_Bluetooth Bluetooth = &_Bluetooth_0;

    Bluetooth->Init = Bluetooth_Init;
    Bluetooth->Configure_Mode = Bluetooth_Configure_Mode;
    Bluetooth->Configure_Param_Len = Bluetooth_Configure_Param_Len;
    Bluetooth->Configure_Callback = Bluetooth_Configure_Callback;
    Bluetooth->Send_Datas = Bluetooth_Send_Datas;
    Bluetooth->Bind_Param_With_Id = Bluetooth_Bind_Param_With_Id;
    Bluetooth->Modify_Param_With_Id = Bluetooth_Modify_Param_With_Id;

    return Bluetooth;
}

/**
 * @brief 初始化蓝牙对象
 *
 * @param this 蓝牙对象
 * @param index 串口索引号
 * @param float_len 浮点数参数个数
 * @param int_len 整数参数个数
 */
void Bluetooth_Init(pClass_Bluetooth this, uint8_t uart_index, uint8_t param_len)
{
    this->mode = STRING; // 默认调试模式为字符串模式

    this->UART_INST = Get_UART_INST(uart_index);
    if (this->UART_INST == NULL)
    {
        return;
    }

    this->param_len = param_len;

    this->param_list = (float **)malloc(this->param_len * sizeof(float *));
    if (this->param_list == NULL)
    {
        return;
    }

    this->is_inited = true;
}

/**
 * @brief 配置蓝牙对象的调试模式
 *
 * @param this 蓝牙对象
 * @param mode 调试模式
 */
void Bluetooth_Configure_Mode(pClass_Bluetooth this, DEBUG_MODE mode)
{
    this->mode = mode;
}

/**
 * @brief 配置蓝牙对象的参数长度
 *
 * @param this 蓝牙对象
 * @param param_len 参数长度
 */
void Bluetooth_Configure_Param_Len(pClass_Bluetooth this, uint8_t param_len)
{
    this->param_len = param_len;
}

/**
 * @brief 配置蓝牙对象的回调函数
 *
 * @param this 蓝牙对象
 * @param callback 回调函数
 */
void Bluetooth_Configure_Callback(pClass_Bluetooth this, void (*callback)(pClass_UART this))
{
    this->UART_INST->UART_INST_DataProcess = callback;
}

/**
 * @brief 发送一组数据
 *
 * @param this 蓝牙对象
 * @param data 发送的数据(仅对STRING模式有效)
 * @param size 数据大小（仅对STRING模式有效）
 *
 */
void Bluetooth_Send_Datas(pClass_Bluetooth this, uint8_t *datas, size_t size)
{
    switch (this->mode)
    {
    case STRING:
        this->UART_INST->Send_Datas(this->UART_INST, datas, size);
        break;
    case WAVE:
        for (int i = 0; i < this->param_len; i++)
        {
            this->UART_INST->Send_Datas(this->UART_INST, (uint8_t *)(this->param_list[i]), sizeof(float)); // 发送浮点数参数
        }
        this->UART_INST->Send_Datas(this->UART_INST, (uint8_t *)WAVE_TAIL, sizeof(WAVE_TAIL)); // 发送WAVE数据尾
        break;
    case CUSTOM:
        break;
    default:
        break;
    }
}

/**
 * @brief 绑定参数
 *
 * @param this 蓝牙对象
 * @param id 参数ID
 * @param input_param 输入参数指针
 */
void Bluetooth_Bind_Param_With_Id(pClass_Bluetooth this, uint8_t id, float *input_param)
{
    if (id >= this->param_len)
        return;

    this->param_list[id] = input_param;
}

/**
 * @brief 修改参数
 *
 * @param this 蓝牙对象
 * @param id 参数ID
 * @param value 修改的值
 */
void Bluetooth_Modify_Param_With_Id(pClass_Bluetooth this, uint8_t id, float value)
{
    if (id >= this->param_len)
        return;

    *(this->param_list[id]) = value;
}

/**
 * @brief 蓝牙0接收回调
 *
 * @param this 蓝牙对象
 */
void Bluetooth_0_Rx_Callback(pClass_UART this)
{
    static uint8_t rxbuffer[BLUETOOTH_RX_LEN_MAX] = {0};
    static uint8_t rx_len = 0;

    if (this->current_byte != '?')
    {
        if (rx_len < BLUETOOTH_RX_LEN_MAX)
        {
            rxbuffer[rx_len++] = this->current_byte; // 接收数据
        }
        else
        {
            rx_len = 0; // 清空接收数据
        }
    }
    else
    {
        rxbuffer[rx_len++] = '\0';
        int id = 0;
        float value = 0.0f;
        sscanf((char *)(rxbuffer), "%d=%f", &id, &value);
        if (_Bluetooth_0.is_inited)
            _Bluetooth_0.Modify_Param_With_Id(&_Bluetooth_0, id, value); // 修改参数

        // 清空接收数据
        for (int i = 0; i < rx_len; i++)
        {
            rxbuffer[i] = 0;
        }
        rx_len = 0;
    }
}
