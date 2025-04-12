#include "Base_Modules/imu.h"
#include "Base_Modules/reminder.h"

void Handle_IMU_Data_Bag(pClass_UART this)
{
    static float first_yaw = 0.0f;
    if(this->rxbuffer[0] == 0x55 && this->rxbuffer[1] == 0x53){
        uint8_t crc = 0;
        for (int i = 0; i < this->rx_len - 1; i++){
            crc += this->rxbuffer[i];
        }
        if(crc == this->rxbuffer[this->rx_len - 1]){
            // LED(TOGGLE);
            if(!first_yaw){
                first_yaw = (short)((short)(this->rxbuffer[7] << 8) | this->rxbuffer[6]) / 32768.0f * PI;
            }else{
                this->Modify_Param_With_Id(this, 0, TransAngleInPI((short)((short)(this->rxbuffer[7] << 8) | this->rxbuffer[6]) / 32768.0f * PI - first_yaw));
            }
        }
    }
}
void IMU_Rx_Callback(pClass_UART this)
{
    static bool find_bag = false; // 是否找到数据包

    if (this->current_byte == 0x55 && !find_bag)
    {
        find_bag = true;
        this->rx_len = 0;
    }

    if(find_bag){
        this->rxbuffer[this->rx_len++] = this->current_byte;
        if(this->rx_len > 1){
            if(this->rxbuffer[0] != 0x55 || this->rxbuffer[1] != 0x53){
                find_bag = false;
                this->rx_len = 0;
            }
        }
    }

    if(this->rx_len == 11){
        Handle_IMU_Data_Bag(this);
        this->rx_len = 0;
        find_bag = false;
    }
}