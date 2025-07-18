#include "Base_Modules/hmi.h"
#include "string.h"

#ifdef USE_HMI

/**
 * @brief 接收串口屏数据回调函数
 * 
 * @param this 串口对象
 */
void HMI_Rx_Callback(pClass_UART this)
{
    if (this->current_byte == 0xFF||this->current_byte == 0x12||this->current_byte == 0x1A||this->current_byte == 0x00)                 //过滤串口屏返回的错误提示
	{
		return;
	}
	if (this->current_byte != '?')
    {  
		if (this->rx_len < HMI_RX_LEN_MAX)
        {
            this->rxbuffer[this->rx_len++] = this->current_byte;
        }
        else
        {
            this->rx_len = 0;
        }
    }
    else
    {
        this->rxbuffer[this->rx_len++] = '\0';
        if (strcmp((char *)this->rxbuffer, "HMI_WATCH") == 0)
        {
			this->mode = HMI_WATCH;
        }
		else if (strcmp((char *)this->rxbuffer, "HMI_STOP") == 0)
		{
			this->mode = HMI_STOP;
		}
        else if (strcmp((char *)this->rxbuffer, "HMI_WAVE") == 0)
        {
			this->mode = HMI_WAVE;
        }
        else if (strcmp((char *)this->rxbuffer, "task1") == 0)
        {
			
        } 
        else if (strcmp((char *)this->rxbuffer, "task2") == 0)
        {
			
        } 
        else if (strcmp((char *)this->rxbuffer, "task3") == 0)
        {
			
        } 
        else if (strcmp((char *)this->rxbuffer, "task4") == 0)
        {
			
        } 
        else if (strcmp((char *)this->rxbuffer, "task5") == 0)
        {
			
        } 
        else if (strcmp((char *)this->rxbuffer, "task6") == 0)
        {
			
        } 
        else
        {
			int id = 0;
            float value = 0.0f;
            sscanf((char *)this->rxbuffer,"%d=%f",&id,&value);
            this->Modify_Param_With_Id(this,id,value);
        }
		this->Clear_RxBuffer(this);
	}
}
#endif