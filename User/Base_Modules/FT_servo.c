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
        printf("FT Servo ID:%d ping failed!\n", this->Servo_ID);
        return;
    }

    this->Pos = 5000;
    this->Speed = 0;
    this->Load = 0;
    this->Temper = 0;
    this->Voltage = 0;
    this->Move = 0;
    this->Current = 0;

    retry = 20;
    while (this->Pos == 5000 && retry--)
    {
        this->FeedBack(this);
    }
    if (!retry)
    {
        printf("FT Servo ID:%d feedback failed!\n", this->Servo_ID);
        return;
    }

    this->Reset_Pos = this->Pos;
    this->Pos_Mode = Pos_Mode;

    switch (this->Pos_Mode)
    {
    case POS_MODE_ABSOLUTE:
        break;
    case POS_MODE_RELATIVE:
        this->Set_Mid_Pos(this);
        break;
    default:
        break;
    }
    this->Set_Max_Pos(this, Max_Pos);
    this->Set_Min_Pos(this, Min_Pos);
    this->Set_Max_Spd(this, Max_Spd);
    this->Set_Min_Spd(this, Min_Spd);
    this->Set_Max_Acc(this, Max_Acc);
    this->Set_Min_Acc(this, Min_Acc);
    this->Target_Pos = this->Pos;
    this->Target_Spd = 0;
    this->Target_Acc = 0;
}

void FT_Servo_Set_Max_Pos(pClass_FT_Servo this, uint16_t pos)
{
    uint16_t pos_temp = pos;
    Math_Constrain_uint16(&pos_temp, 0, 4095);
    this->Max_Pos = pos;
}
void FT_Servo_Set_Min_Pos(pClass_FT_Servo this, uint16_t pos)
{
    uint16_t pos_temp = pos;
    Math_Constrain_uint16(&pos_temp, 0, 4095);
    this->Min_Pos = pos;
}

void FT_Servo_Set_Max_Spd(pClass_FT_Servo this, uint16_t spd)
{
    uint16_t spd_temp = spd;
    Math_Constrain_uint16(&spd_temp, 0, 90);
    this->Max_Spd = spd;
}

void FT_Servo_Set_Min_Spd(pClass_FT_Servo this, uint16_t spd)
{
    uint16_t spd_temp = spd;
    Math_Constrain_uint16(&spd_temp, 0, 90);
    this->Min_Spd = spd;
}

void FT_Servo_Set_Max_Acc(pClass_FT_Servo this, uint16_t acc)
{
    uint16_t acc_temp = acc;
    Math_Constrain_uint16(&acc_temp, 0, 250);
    this->Max_Acc = acc;
}
void FT_Servo_Set_Min_Acc(pClass_FT_Servo this, uint16_t acc)
{
    uint16_t acc_temp = acc;
    Math_Constrain_uint16(&acc_temp, 0, 250);
    this->Min_Acc = acc;
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
        printf("Servo ID:%d\n", this->Servo_ID);
        return true;
    }
    else
    {
        printf("Ping servo ID error!\n");
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
    this->Target_Pos = pos;
    uint16_t spd_temp = spd;
    Math_Constrain_uint16(&spd_temp, this->Min_Spd, this->Max_Spd);
    this->Target_Spd = spd;
    uint16_t acc_temp = acc;
    Math_Constrain_uint16(&acc_temp, this->Min_Acc, this->Max_Acc);
    this->Target_Acc = acc;
}

uint8_t ID[2];
int16_t Position[2];
uint16_t Speed[2];
uint8_t ACC[2];

void FT_Servo_Update(void)
{
    if (_FT_SERVO_1.Status == ONLINE || _FT_SERVO_1.Status == PROTECTED)
    {
        _FT_SERVO_1.FeedBack(&_FT_SERVO_1); // 更新舵机反馈信息
        _FT_SERVO_1.Safety_Check(&_FT_SERVO_1);
        if (_FT_SERVO_1.Status == PROTECTED)
        {
            _FT_SERVO_1.Set_Target_Status(&_FT_SERVO_1, _FT_SERVO_1.Reset_Pos, 30, 50);
        }
        {
            ID[0] = _FT_SERVO_1.Servo_ID;
            Position[0] = _FT_SERVO_1.Target_Pos;
            Speed[0] = _FT_SERVO_1.Target_Spd;
            ACC[0] = _FT_SERVO_1.Target_Acc;
        }
    }
    else
    {
        ID[0] = 0; // 舵机离线
        Position[0] = 0;
        Speed[0] = 0;
        ACC[0] = 0;
    }

    if (_FT_SERVO_2.Status == ONLINE || _FT_SERVO_2.Status == PROTECTED)
    {
        _FT_SERVO_2.FeedBack(&_FT_SERVO_2); // 更新舵机反馈信息
        _FT_SERVO_2.Safety_Check(&_FT_SERVO_2);
        if (_FT_SERVO_2.Status == PROTECTED)
        {
            _FT_SERVO_2.Set_Target_Status(&_FT_SERVO_2, _FT_SERVO_2.Reset_Pos, 30, 50);
        }

        {
            ID[1] = _FT_SERVO_2.Servo_ID;
            Position[1] = _FT_SERVO_2.Target_Pos;
            Speed[1] = _FT_SERVO_2.Target_Spd;
            ACC[1] = _FT_SERVO_2.Target_Acc;
        }
    }
    else
    {
        ID[1] = 0; // 舵机离线
        Position[1] = 0;
        Speed[1] = 0;
        ACC[1] = 0;
    }

    SyncWritePosEx(ID, 2, Position, Speed, ACC);
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
    if (nLen > FT_SERVO_RX_LEN_MAX)
    {
        return 0; // 如果请求长度大于缓冲区长度，直接返回
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
    delay_cycles(80 * 1000 * 10);
}

#endif