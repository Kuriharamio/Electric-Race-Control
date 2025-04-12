/**
 * @file uart.c
 * @author KuriharaMio
 * @brief 串口类实现文件
 * @version 0.1
 * @date 2025-04-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "BSP/uart.h"

static Class_UART _UART_0_INST = {0}; // 串口0实例
static Class_UART _UART_1_INST = {0}; // 串口1实例
static Class_UART _UART_2_INST = {0}; // 串口2实例

static uint8_t WAVE_TAIL[4] = {0x00, 0x00, 0x80, 0x7f}; // WAVE数据尾(JustFloat)
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
		return &_UART_0_INST;
		break;
	case 1:
		return &_UART_1_INST;
		break;
	case 2:
		return &_UART_2_INST;
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
	case 2:
		this = &_UART_2_INST;
		break;
	default:
		return NULL;
	}

	this->index = index; // 设置索引号

	this->Init = UART_Init;						// 初始化函数
	this->Send_Bit = UART_Send_Bit;				// 发送数据函数
	this->Send_Datas = UART_Send_Datas;			// 发送字符串函数
	this->Clear_RxBuffer = UART_Clear_RxBuffer; // 清除接收缓冲区
	this->UART_INST_DataProcess = NULL;			// 串口中断处理函数
	this->Custom_Send_Datas = NULL;				// 自定义发送数据函数
	this->Send = UART_Send;						// 发送数据函数

	this->Configure_Mode = UART_Configure_Mode;				// 配置调试模式
	this->Configure_Param_Len = UART_Configure_Param_Len;	// 配置参数长度
	this->Configure_Callback = UART_Configure_Callback;		// 配置回调函数
	this->Bind_Param_With_Id = UART_Bind_Param_With_Id;		// 绑定参数
	this->Modify_Param_With_Id = UART_Modify_Param_With_Id; // 修改参数

	return this;
}

/**
 * @brief 串口对象初始化
 *
 * @param this
 */
void UART_Init(pClass_UART this, uint8_t rx_max_len, uint8_t param_len)
{
	NVIC_ClearPendingIRQ(Get_UART_IRQN_From_Index(this->index));
	NVIC_EnableIRQ(Get_UART_IRQN_From_Index(this->index));

	this->mode = DEBUG_STRING;
	this->param_len = param_len;											// 设置参数个数
	this->param_list = (float **)malloc(this->param_len * sizeof(float *)); // 分配参数列表内存
	if (this->param_list == NULL)
	{
		return; // 分配失败
	}

	this->rx_max_len = rx_max_len;									  // 设置接收数据最大长度
	this->rxbuffer = (uint8_t *)malloc(rx_max_len * sizeof(uint8_t)); // 分配接收数据缓冲区
	if (this->rxbuffer == NULL)
	{
		return; // 分配失败
	}

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

void UART_Send(pClass_UART this, uint8_t *datas, size_t size)
{
	if (this->mode == DEBUG_WAVE)
	{
		for (int i = 0; i < this->param_len; i++)
		{
			this->Send_Datas(this, (uint8_t *)(this->param_list[i]), sizeof(float)); // 发送浮点数参数
		}
		this->Send_Datas(this, (uint8_t *)WAVE_TAIL, sizeof(WAVE_TAIL)); // 发送WAVE数据尾
	}
	else if (this->mode == DEBUG_STRING)
	{
		this->Send_Datas(this, datas, size); // 发送数据
	}
	else if (this->mode == CUSTOM)
	{
		if (this->Custom_Send_Datas != NULL)
		{
			this->Custom_Send_Datas(datas, size); // 发送数据
		}
		else
		{
			this->Send_Datas(this, datas, size); // 发送数据
		}
	}
}

/**
 * @brief 配置调试模式
 *
 * @param this 串口对象
 * @param mode 调试模式
 */
void UART_Configure_Mode(pClass_UART this, UART_MODE mode)
{
	this->mode = mode;
}

/**
 * @brief 配置参数长度
 *
 * @param this 串口对象
 * @param param_len 参数长度
 */
void UART_Configure_Param_Len(pClass_UART this, uint8_t param_len)
{
	this->param_len = param_len;
}

/**
 * @brief 配置串口对象的自定义发送数据函数
 *
 * @param this 串口对象
 * @param func 自定义发送数据函数
 */
void UART_Configure_Custom_Send_Datas(pClass_UART this, void (*func)(uint8_t *datas, size_t size))
{
	this->Custom_Send_Datas = func; // 设置自定义发送数据函数
}

/**
 * @brief 配置串口对象的回调函数
 *
 * @param this 串口对象
 * @param callback 回调函数
 */
void UART_Configure_Callback(pClass_UART this, void (*callback)(pClass_UART this))
{
	this->UART_INST_DataProcess = callback; // 设置回调函数
}

/**
 * @brief 绑定参数
 *
 * @param this 串口对象
 * @param id 参数ID
 * @param input_param 输入参数指针
 */
void UART_Bind_Param_With_Id(pClass_UART this, uint8_t id, float *input_param)
{
	if (id >= this->param_len)
		return;

	this->param_list[id] = input_param;
}

/**
 * @brief 修改参数
 *
 * @param this 串口对象
 * @param id 参数ID
 * @param value 修改的值
 */
void UART_Modify_Param_With_Id(pClass_UART this, uint8_t id, float value)
{
	if (id >= this->param_len)
		return;

	*(this->param_list[id]) = value;
}

/**
 * @brief 清除接收缓冲区
 *
 * @param this 串口对象
 */
void UART_Clear_RxBuffer(pClass_UART this)
{
	// 清空接收数据
	for (int i = 0; i < this->rx_len; i++)
	{
		this->rxbuffer[i] = 0;
	}
	this->rx_len = 0;
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
	case 2:
		return UART_2_INST_INT_IRQN;
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
	case 2:
		return UART_2_INST;
	default:
		return NULL;
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
		DL_UART_transmitData(UART_0_INST,receivedData);
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

/**
 * @brief 串口2中断处理函数
 *
 */
void UART_2_INST_IRQHandler(void)
{
	if (!_UART_2_INST.is_inited)
		return;

	uint8_t receivedData = 0;
	// 如果产生了串口中断
	switch (DL_UART_getPendingInterrupt(UART_2_INST))
	{
	case DL_UART_IIDX_RX: // 如果是接收中断
	{
		// 接收发送过来的数据保存
		receivedData = DL_UART_Main_receiveData(UART_2_INST);
		_UART_2_INST.current_byte = receivedData; // 保存当前接收的字节
		if (_UART_2_INST.UART_INST_DataProcess != NULL)
		{
			_UART_2_INST.UART_INST_DataProcess(&_UART_2_INST); // 调用中断处理函数
		}
	}
	break;
	default:
		break;
	}
}
