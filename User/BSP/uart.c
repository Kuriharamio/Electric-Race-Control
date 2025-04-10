/**
 * @file uart.c
 * @author KuriharaMio
 * @brief
 * @version 0.1
 * @date 2025-04-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "BSP/uart.h"

static Class_UART _UART_0_INST = {0}; // 串口0实例
static Class_UART _UART_1_INST = {0}; // 串口1实例

/**
 * @brief 获取UART对象实例
 *
 * @param index
 * @return pClass_UART
 */
pClass_UART Get_UART_INST(uint8_t index)
{
	switch (index)
	{
	case 0:
		if (!_UART_0_INST.is_configured && _UART_0_INST.is_inited) // 如果没有配置过
		{
			_UART_0_INST.is_configured = true; // 设置为已配置
			return &_UART_0_INST;
		}
		break;
	case 1:
		if (!_UART_1_INST.is_configured && _UART_1_INST.is_inited) // 如果没有配置过
		{
			_UART_1_INST.is_configured = true; // 设置为已配置
			return &_UART_1_INST;
		}
		break;
	default:
		return NULL;
	}
	return NULL;
}

pClass_UART Create_UART(uint8_t index)
{
	pClass_UART this = NULL;
	switch (index)
	{
	case 0:
		this = &_UART_0_INST;
		break;
	case 1:
		this = &_UART_1_INST;
		break;
	default:
		return NULL;
	}

	this->index = index; // 设置索引号

	this->Init = UART_Init;					// 初始化函数
	this->Send_Bit = UART_Send_Bit;			// 发送数据函数
	this->Send_Datas = UART_Send_Datas;		// 发送字符串函数
	this->UART_INST_DataProcess = NULL;		// 串口中断处理函数

	return this;
}

/**
 * @brief 串口对象初始化
 *
 * @param this
 */
void UART_Init(pClass_UART this)
{
	NVIC_ClearPendingIRQ(Get_UART_IRQN_From_Index(this->index));
	NVIC_EnableIRQ(Get_UART_IRQN_From_Index(this->index));

	this->current_byte = 0;

	this->is_inited = true;
}

/**
 * @brief 串口发送单个字节
 *
 * @param this
 * @param data
 */
void UART_Send_Bit(pClass_UART this, uint8_t data)
{
	while (DL_UART_isBusy(Get_UART_INST_From_Index(this->index)) == true)
	{
		// 等待串口空闲
	};
	DL_UART_Main_transmitData(Get_UART_INST_From_Index(this->index), data); // 发送数据
}

/**
 * @brief 串口发送一组数据
 *
 * @param this
 * @param datas
 * @param size
 */
void UART_Send_Datas(pClass_UART this, uint8_t *datas, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		this->Send_Bit(this, datas[i]); // 发送数据
	}
}


/**
 * @brief 由索引号获取IRQN
 *
 * @param index
 * @return IRQn_Type
 */
IRQn_Type Get_UART_IRQN_From_Index(uint8_t index)
{
	switch (index)
	{
	case 0:
		return UART_0_INST_INT_IRQN;
	case 1:
		return UART_1_INST_INT_IRQN;
	default:
		return -1;
	}
}

/**
 * @brief 由索引号获取UART
 *
 * @param index
 * @return UART_Regs*
 */
UART_Regs *Get_UART_INST_From_Index(uint8_t index)
{
	switch (index)
	{
	case 0:
		return UART_0_INST;
	case 1:
		return UART_1_INST;
	default:
		return NULL;
	}
}

#include "Base_Modules/led.h"
/**
 * @brief 串口0中断处理函数
 *
 */
void UART_0_INST_IRQHandler(void)
{
	if (!_UART_0_INST.is_inited)
		return;

	uint8_t receivedData = 0;
	// 如果产生了串口中断
	switch (DL_UART_getPendingInterrupt(UART_0_INST))
	{
	case DL_UART_IIDX_RX: // 如果是接收中断
	{
		// 接收发送过来的数据保存
		receivedData = DL_UART_Main_receiveData(UART_0_INST);
		LED(TOGGLE);
		_UART_0_INST.current_byte = receivedData; // 保存当前接收的字节
		if (_UART_0_INST.UART_INST_DataProcess != NULL)
		{
			_UART_0_INST.UART_INST_DataProcess(&_UART_0_INST); // 调用中断处理函数
		}
	}
	break;
	default:
		break;
	}
}

/**
 * @brief 串口1中断处理函数
 *
 */
void UART_1_INST_IRQHandler(void)
{
	if (!_UART_1_INST.is_inited)
		return;

	uint8_t receivedData = 0;
	// 如果产生了串口中断
	switch (DL_UART_getPendingInterrupt(UART_1_INST))
	{
	case DL_UART_IIDX_RX: // 如果是接收中断
	{
		// 接收发送过来的数据保存
		receivedData = DL_UART_Main_receiveData(UART_1_INST);
		_UART_1_INST.current_byte = receivedData; // 保存当前接收的字节
		if (_UART_1_INST.UART_INST_DataProcess != NULL)
		{
			_UART_1_INST.UART_INST_DataProcess(&_UART_1_INST); // 调用中断处理函数
		}
	}
	break;
	default:
		break;
	}
}
