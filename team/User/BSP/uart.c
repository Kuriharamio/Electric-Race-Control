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
	this->Clear_Buffer = UART_Clear_Buffer; // 清空接收数据函数
	this->UART_INST_DataProcess = NULL;		// 串口中断处理函数

	return this;
}

/**
 * @brief 串口对象初始化
 *
 * @param this
 */
void UART_Init(pClass_UART this, UART_RX_MODE mode)
{
	NVIC_ClearPendingIRQ(Get_UART_IRQN_From_Index(this->index));
	NVIC_EnableIRQ(Get_UART_IRQN_From_Index(this->index));

	this->rx_len = 0;
	this->is_received = false;
	this->current_byte = 0;
	this->mode = mode; 

	this->rx_len_max = Get_UART_RX_Len_Max_From_Index(this->index);
	this->rxbuffer = (uint8_t *)malloc(this->rx_len_max * sizeof(uint8_t));
	if (this->rxbuffer == NULL)
	{
		return;
	}

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
 * @brief 清空接收数据函数
 *
 * @param this
 */
void UART_Clear_Buffer(pClass_UART this)
{
	this->rx_len = 0;		   // 清空接收数据长度
	this->is_received = false; // 清空接收标志位
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

/**
 * @brief 由索引号获取接收数据最大长度
 *
 * @param index
 * @return int
 */
int Get_UART_RX_Len_Max_From_Index(uint8_t index)
{
	switch (index)
	{
	case 0:
		return RX_LEN_MAX_0;
	case 1:
		return RX_LEN_MAX_1;
	default:
		return -1;
	}
}

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
		if (_UART_0_INST.mode == PACKET) // 如果是数据包模式
		{
			_UART_0_INST.current_byte = receivedData; // 保存当前接收的字节
			if (_UART_0_INST.UART_INST_DataProcess != NULL)
			{
				_UART_0_INST.UART_INST_DataProcess(&_UART_0_INST); // 调用中断处理函数
			}
		}
		else
		{
			// 检查缓冲区是否已满
			if (_UART_0_INST.rx_len < RX_LEN_MAX_0 - 1)
			{
				_UART_0_INST.rxbuffer[_UART_0_INST.rx_len++] = receivedData;
			}
			else
			{
				_UART_0_INST.rx_len = 0;
			}
			// 标记接收标志
			_UART_0_INST.is_received = true;
//			if (_UART_0_INST.UART_INST_DataProcess != NULL)
//			{
//				_UART_0_INST.UART_INST_DataProcess(&_UART_0_INST); // 调用中断处理函数
//			}
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
		if (_UART_1_INST.mode == PACKET)		  // 如果是数据包模式
		{
			if (_UART_1_INST.UART_INST_DataProcess != NULL)
			{
				_UART_1_INST.UART_INST_DataProcess(&_UART_1_INST); // 调用中断处理函数
			}
		}
		else
		{
			// 检查缓冲区是否已满
			if (_UART_1_INST.rx_len < RX_LEN_MAX_1 - 1)
			{
				_UART_1_INST.rxbuffer[_UART_1_INST.rx_len++] = receivedData;
			}
			else
			{
				_UART_1_INST.rx_len = 0;
			}
			// 标记接收标志
			_UART_1_INST.is_received = true;
//			if (_UART_1_INST.UART_INST_DataProcess != NULL)
//			{
//				_UART_1_INST.UART_INST_DataProcess(&_UART_1_INST); // 调用中断处理函数
//			}
		}
	}
	break;
	default:
		break;
	}
}
