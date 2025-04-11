#include "Base_Modules/k230_serial.h"

static RxState Rx_State = STATE_WAIT_HEADER;//接收状态标志位
static uint8_t Rx_Data_Len = 0;//数据长度
static uint8_t Rx_BCC = 0;//接收到的BCC校验位

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
			if (Rx_Data_Len > MAX_DATA_LEN)
			{
				//数据字段长度大于规定长度重置标志位
				Rx_State = STATE_WAIT_HEADER;
			}
			else
			{
				//合法字长即将标志位转为STATE_WAIT_DATA
				Rx_State = STATE_WAIT_DATA;
				this->rx_len = 0;
			}
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
				int id = 0;
				float value = 0.0f;
				sscanf((char *)(this->rxbuffer), "%d=%f", &id, &value);
				this->Modify_Param_With_Id(this, id, value); // 修改参数
			}
			//接收完重置标志位
			Rx_State = STATE_WAIT_HEADER;
			//this->Clear_RxBuffer(this);
			break;
	}
    
}
