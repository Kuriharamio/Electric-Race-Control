/**
 * @file FT_Servo.c
 * @author KuriharaMio
 * @brief 飞特舵机库
 * @version 0.1
 * @date 2025-07-07
 * @note 关于限幅：可以更改舵机中存储的限幅位，但我只在单片机端进行了限幅
 *       关于控制方式：目前只使用了位置控制，还可以使用速度控制（按一定速度持续旋转，这个库没有使用）
 *       若要实现激光跟随等功能，还需要加一个跟随PID，可参考普通舵机库 servo.c
 *       待优化：测试是否可以同时获取两个舵机的信息
 *       http://doc.feetech.cn/#/prodinfodownload?srcType=FT-SMS-STS-emanual-229f4476422d4059abfb1cb0
 * @copyright Copyright (c) 2025
 *
 */

#include "FT_servo.h"

#ifdef USE_FT_SERVO
// <<<<<<<<<   实例声明     <<<<<<<<<<<
Class_FT_Servo _FT_SERVO_1 = {1};
Class_FT_Servo _FT_SERVO_2 = {2};
// >>>>>>>>>   实例声明     >>>>>>>>>>>
uint8_t Rx_Buffer[FT_SERVO_RX_LEN_MAX];
int Rx_Len;
uint8_t Last_Write_Index;
uint8_t Last_Read_Index;

pClass_FT_Servo Create_FT_Servo(uint8_t ID)
{
    pClass_FT_Servo temp_ptr = NULL;
    switch (ID)
    {
    case 1:
        temp_ptr = &_FT_SERVO_1;
        temp_ptr->Servo_ID = 1;
        break;
    case 2:
        temp_ptr = &_FT_SERVO_2;
        temp_ptr->Servo_ID = 2;
        break;
    default:
        break;
    }

    temp_ptr->Init = FT_Servo_Init;
    temp_ptr->Set_Mid_Pos = FT_Servo_Set_Mid_Pos;
    temp_ptr->Set_Min_Pos = FT_Servo_Set_Min_Pos;
    temp_ptr->Set_Max_Pos = FT_Servo_Set_Max_Pos;
    temp_ptr->Set_Min_Spd = FT_Servo_Set_Min_Spd;
    temp_ptr->Set_Max_Spd = FT_Servo_Set_Max_Spd;
    temp_ptr->Set_Min_Acc = FT_Servo_Set_Min_Acc;
    temp_ptr->Set_Max_Acc = FT_Servo_Set_Max_Acc;
    temp_ptr->Set_Target_Status = FT_Servo_Set_Target_Status;
    temp_ptr->Ping = FT_Servo_Ping;
    temp_ptr->FeedBack = FT_Servo_FeedBack;
    temp_ptr->Safety_Check = FT_Servo_Safety_Check;
    temp_ptr->Update_PID = FT_Servo_Update_PID;

    temp_ptr->PID = create_PID();

    return temp_ptr;
}
pClass_FT_Servo Get_FT_Servo_Handle(uint8_t ID)
{
    pClass_FT_Servo temp_ptr = NULL;
    switch (ID)
    {
    case 1:
        temp_ptr = &_FT_SERVO_1;
        break;
    case 2:
        temp_ptr = &_FT_SERVO_2;
        break;
    default:
        break;
    }

    return temp_ptr;
}

void FT_Servo_Init(pClass_FT_Servo this, FT_SERVO_POS_MODE Pos_Mode, uint16_t Max_Pos, uint16_t Min_Pos, uint16_t Max_Spd, uint16_t Min_Spd, uint16_t Max_Acc, uint16_t Min_Acc)
{
    this->PID->PID_Init(this->PID, 0.1f, 0.15f, 0.1f, 0.00f, 2047.0f, 2047.0f, PID_SERVO_DELTA_T*3, 0.00f, 10.00f, 30.00f, 30.00f, PID_D_First_DISABLE);

    setEnd(0);              // SMS_STS舵机为小端存储结构
    this->Status = OFFLINE; // 舵机初始状态为离线

    uint8_t retry = 20;
    while (this->Status == OFFLINE && retry--)
    {
        if (this->Ping(this))
        {
            this->Status = ONLINE;
        }
    }
    if (!retry)
    {
#ifdef PRINT_DEBUG
        printf("FT Servo ID:%d ping failed!\n", this->Servo_ID);
#endif
        return;
    }

    this->Pos = 5000;
    this->Speed = 0;
    this->Load = 0;
    this->Temper = 0;
    this->Voltage = 0;
    this->Move = 0;
    this->Current = 0;

    // this->Set_Mid_Pos(this);
    this->PID_Output = 0.0f;

    retry = 255;
    while (this->Pos == 5000 && retry--)
    {
        this->FeedBack(this);
        delay_ms(10);
    }
    if (!retry)
    {
#ifdef PRINT_DEBUG
        printf("FT Servo ID:%d feedback failed!\n", this->Servo_ID);
#endif
        return;
    }else{
#ifdef PRINT_DEBUG
        printf("FT Servo ID:%d feedback success!\tPose: %d\n", this->Servo_ID, this->Pos);
#endif
    }

    this->Reset_Pos = this->Pos;
    this->Pos_Mode = Pos_Mode;

    this->Set_Max_Pos(this, Max_Pos);
    this->Set_Min_Pos(this, Min_Pos);
    this->Set_Max_Spd(this, Max_Spd);
    this->Set_Min_Spd(this, Min_Spd);
    this->Set_Max_Acc(this, Max_Acc);
    this->Set_Min_Acc(this, Min_Acc);
    this->Target_Pos = this->Pos;
    this->Target_Spd = 0;
    this->Target_Acc = 0;

    this->is_inited = true;
}
void FT_Servo_Set_Max_Pos(pClass_FT_Servo this, uint16_t pos)
{
    uint16_t pos_temp = pos;
    Math_Constrain_uint16(&pos_temp, 0, 4095);
    this->Max_Pos = pos_temp;
}
void FT_Servo_Set_Min_Pos(pClass_FT_Servo this, uint16_t pos)
{
    uint16_t pos_temp = pos;
    Math_Constrain_uint16(&pos_temp, 0, 4095);
    this->Min_Pos = pos_temp;
}
void FT_Servo_Set_Max_Spd(pClass_FT_Servo this, uint16_t spd)
{
    uint16_t spd_temp = spd;
    Math_Constrain_uint16(&spd_temp, 0, 90);
    this->Max_Spd = spd_temp;
}
void FT_Servo_Set_Min_Spd(pClass_FT_Servo this, uint16_t spd)
{
    uint16_t spd_temp = spd;
    Math_Constrain_uint16(&spd_temp, 0, 90);
    this->Min_Spd = spd_temp;
}
void FT_Servo_Set_Max_Acc(pClass_FT_Servo this, uint16_t acc)
{
    uint16_t acc_temp = acc;
    Math_Constrain_uint16(&acc_temp, 0, 250);
    this->Max_Acc = acc_temp;
}
void FT_Servo_Set_Min_Acc(pClass_FT_Servo this, uint16_t acc)
{
    uint16_t acc_temp = acc;
    Math_Constrain_uint16(&acc_temp, 0, 250);
    this->Min_Acc = acc_temp;
}
void FT_Servo_Set_Mid_Pos(pClass_FT_Servo this)
{
    CalibrationOfs(this->Servo_ID);
}
bool FT_Servo_Ping(pClass_FT_Servo this)
{
    Ping(this->Servo_ID);
    if (!getLastError())
    {
#ifdef PRINT_DEBUG
        printf("Ping Servo ID:%d success!\n", this->Servo_ID);
        #endif
        return true;
    }
    else
    {
        // printf("Ping servo ID error!\n");
        return false;
    }
}
void FT_Servo_FeedBack(pClass_FT_Servo this)
{
    FeedBack(this->Servo_ID);
    if (!getLastError())
    {
        this->Pos = ReadPos(-1);
        this->Speed = ReadSpeed(-1);
        this->Load = ReadLoad(-1);
        this->Voltage = ReadVoltage(-1);
        this->Temper = ReadTemper(-1);
        this->Move = ReadMove(-1);
        this->Current = ReadCurrent(-1);
    }else{
        // printf("Error: %d\n", getLastError());
    }
}
void FT_Servo_Safety_Check(pClass_FT_Servo this)
{
    if (this->Current > PROTECT_CURRENT)
    {
        this->Status = PROTECTED;
    }
}
void FT_Servo_Set_Target_Status(pClass_FT_Servo this, uint16_t pos, uint16_t spd, uint16_t acc)
{
    uint16_t pos_temp = pos;
    Math_Constrain_uint16(&pos_temp, this->Min_Pos, this->Max_Pos);
    this->Target_Pos = pos_temp;
    uint16_t spd_temp = spd;
    Math_Constrain_uint16(&spd_temp, this->Min_Spd, this->Max_Spd);
    this->Target_Spd = spd_temp;
    uint16_t acc_temp = acc;
    Math_Constrain_uint16(&acc_temp, this->Min_Acc, this->Max_Acc);
    this->Target_Acc = acc_temp;
}
void FT_Servo_Update_PID(pClass_FT_Servo this)
{
    if (this->Status != ONLINE)
        return;

    this->PID->Set_Target(this->PID, -this->Error);
    this->PID->Set_Now(this->PID, 0);
    this->PID->Update_Value(this->PID);

    this->PID_Output = this->PID->Get_PID_Out(this->PID);
    this->Set_Target_Status(this, (uint16_t)(this->PID_Output) + this->Pos, 40, 20);
}
void FT_Servo_Data_Process(pClass_UART this)
{
    Rx_Buffer[Last_Write_Index] = this->current_byte;
    Rx_Len++;
    Last_Write_Index++;
    if (Last_Write_Index >= FT_SERVO_RX_LEN_MAX)
    {
        Last_Write_Index = 0; // 防止缓冲区溢出
    }
}

// FT舵机串口指令发送函数
void ftUart_Send(uint8_t *nDat, int nLen)
{
    pClass_UART UARTptr = Get_UART_INST(FT_SERVO_UART_INDEX);
    UARTptr->Send_Datas(UARTptr, nDat, nLen);
}

// FT舵机串口指令应答接收函数
int ftUart_Read(uint8_t *nDat, int nLen)
{
    if (nLen > Rx_Len)
    {
        return 0; // 如果请求长度大于缓冲区长度，直接返回
        // delay_us(80);
    }

    for (int i = 0; i < nLen; i++)
    {
        nDat[i] = Rx_Buffer[Last_Read_Index++];
        // printf("Read data: %d\n", nDat[i]);
        if (Last_Read_Index >= FT_SERVO_RX_LEN_MAX)
        {
            Last_Read_Index = 0;
        }
    }
    Rx_Len -= nLen; // 更新剩余长度
    return nLen;
}

// FT舵机总线切换延时，时间大于10us
void ftBus_Delay(void)
{
    delay_cycles(80 * 1000);
}

#endif