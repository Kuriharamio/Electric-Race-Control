#include "Base_Modules/k230_serial.h"

static RxState Rx_State = STATE_WAIT_HEADER;//接收状态标志位
static uint8_t Rx_Data_Len = 0;//数据长度
static uint8_t Rx_BCC = 0;//接收到的BCC校验位

/**
 *@brief 将浮点数转化为大端序
 *
 *@param value
 *@param bytes_out
 */ 
void float_to_big_endian_bytes(float value, uint8_t *bytes_out) 
{
    uint32_t as_int;
    memcpy(&as_int, &value, sizeof(float)); // 把 float 的位复制到 uint32_t

    // 转换为大端序
    bytes_out[0] = (as_int >> 24) & 0xFF;
    bytes_out[1] = (as_int >> 16) & 0xFF;
    bytes_out[2] = (as_int >> 8)  & 0xFF;
    bytes_out[3] = (as_int >> 0)  & 0xFF;
}

/**
 *@brief 将字节数组转化为浮点数
 *
 *@param bytes
 */
float BigEndianBytesToFloat(uint8_t *bytes)
{
    uint32_t temp = 0;
    temp |= ((uint32_t)bytes[0] << 24);
    temp |= ((uint32_t)bytes[1] << 16);
    temp |= ((uint32_t)bytes[2] << 8);
    temp |= ((uint32_t)bytes[3]);

    float f;
    memcpy(&f, &temp, sizeof(f));
    return f;
}
/**
 * @brief BCC校验函数
 *
 * @param data
 * @param len
 *
 *@return BCC校验位
 */
uint8_t Calculate_BCC(uint8_t *data, uint8_t len) 
{
    uint8_t bcc = 0;
    for (uint8_t i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}

/**
 * @brief K230接收回调
 *
 * @param this 串口对象
 */
void K230_Rx_Callback(pClass_UART this)
{	
	
	switch(Rx_State)
	{
		case STATE_WAIT_HEADER:
			if (this->current_byte == FRAME_HEADER)
			{
				//检测到帧头将状态标志位转位STATE_WAIT_LENGTH
				Rx_State = STATE_WAIT_LENGTH;
			}
			break;
		case STATE_WAIT_LENGTH:
			Rx_Data_Len = this->current_byte;

			Rx_State = STATE_WAIT_DATA;
			this->rx_len = 0;
			break;
		case STATE_WAIT_DATA:
			this->rxbuffer[this->rx_len++] = this->current_byte;
			if (this->rx_len>=Rx_Data_Len)
			{
				//接收到规定长度数据字段将标志位转为STATE_WAIT_BCC
				Rx_State = STATE_WAIT_BCC;
			}
			break;
		case STATE_WAIT_BCC:
			Rx_BCC = this->current_byte;
			if (Rx_BCC == Calculate_BCC((uint8_t *)this->rxbuffer,this->rx_len))
			{
				this->rxbuffer[this->rx_len++] = '\0';
				//转化浮点数
				if(Rx_Data_Len == 12)
				{
					float f1 = BigEndianBytesToFloat(&this->rxbuffer[0]);
					float f2 = BigEndianBytesToFloat(&this->rxbuffer[4]);
					float f3 = BigEndianBytesToFloat(&this->rxbuffer[8]);
					this->Modify_Param_With_Id(this, 0, f1); // 修改参数
					this->Modify_Param_With_Id(this, 1, f2); // 修改参数
					this->Modify_Param_With_Id(this, 2, f3); // 修改参数
				}
			}
			//接收完重置标志位
			Rx_State = STATE_WAIT_HEADER;
			this->Clear_RxBuffer(this);
			break;
	}
    
}


/**
 *@brief 向k230发送数据
 *
 *@param this
 *@param datas
 *@param len
 */
void K230_Transmit(pClass_UART this, float *datas, int len) 
{
    int data_len = len * 4;               // 每个 float 4 字节
    int frame_len = 1 + 1 + data_len + 1; // 帧头 + 长度字段 + 数据 + 校验
    uint8_t buffer[frame_len];            // 总帧缓冲区

    buffer[0] = 0xAA;                     // 帧头
    buffer[1] = len;                      // 数据长度字段（float 个数）

    uint8_t *data_ptr = &buffer[2];       // 数据字段起始位置

    // 写入 float 数据（大端）
    for (uint8_t i = 0; i < len; i++) {
        float_to_big_endian_bytes(datas[i], &data_ptr[i * 4]);
    }

    // 计算 BCC（对数据字段部分进行异或）
    uint8_t bcc = Calculate_BCC(data_ptr,data_len);

    buffer[2 + data_len] = bcc;           // 校验位写入帧尾

    // 发送完整帧
    this->Send_Datas(this, buffer, frame_len);
}