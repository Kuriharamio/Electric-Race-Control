#include "Base_Modules/imu.h"

#ifdef USE_IMU
void Handle_IMU_Data_Bag(pClass_UART this)
{
    static uint8_t init_cnt = 0;
    static float first_yaw = 0.0f;
    if (this->rxbuffer[0] == 0x55 && this->rxbuffer[1] == 0x53)
    {
        uint8_t crc = 0;
        for (int i = 0; i < this->rx_len - 1; i++)
        {
            crc += this->rxbuffer[i];
        }
        if (crc == this->rxbuffer[this->rx_len - 1])
        {
            float yaw = (float)((short)(this->rxbuffer[7] << 8) | this->rxbuffer[6]) / 32768.0f * 180.0f;
            if (init_cnt < 10)
            {
                init_cnt++;
                first_yaw += yaw;
            }
            else
            {
                this->Modify_Param_With_Id(this, 0, TransAngleInPI((yaw - first_yaw / 10.0f) / 180.0f * PI));
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

    if (find_bag)
    {
        this->rxbuffer[this->rx_len++] = this->current_byte;
        if (this->rx_len > 1)
        {
            if (this->rxbuffer[0] != 0x55 || this->rxbuffer[1] != 0x53)
            {
                find_bag = false;
                this->rx_len = 0;
            }
        }
    }

    if (this->rx_len == 11)
    {
        Handle_IMU_Data_Bag(this);
        this->rx_len = 0;
        find_bag = false;
    }
}

uint8_t unlock[5] = {0xFF, 0xAA, 0x69, 0x88, 0xB5};
uint8_t start_calibration[5] = {0xFF, 0xAA, 0x01, 0x01, 0x00};
uint8_t stop_calibration[5] = {0xFF, 0xAA, 0x01, 0x00, 0x00};
uint8_t save[5] = {0xFF, 0xAA, 0x00, 0x00, 0x00};
void IMU_Init(void)
{
    pClass_UART uart = Get_UART_INST(IMU_UART_INDEX);
    uart->Send_Datas(uart, unlock, 5);
    delay_ms(200);
    uart->Send_Datas(uart, start_calibration, 5);
    delay_ms(4000);
    uart->Send_Datas(uart, stop_calibration, 5);
    delay_ms(100);
    uart->Send_Datas(uart, save, 5);
    delay_ms(100);
}

#endif