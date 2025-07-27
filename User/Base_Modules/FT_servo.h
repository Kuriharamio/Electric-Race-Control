#ifndef __FT_Servo_H__
#define __FT_Servo_H__

#include "config.h"
#include "SCSLib/SCServo.h"
#include "BSP/uart.h"
#include "Algorithm/pid.h"

typedef enum
{
    POS_MODE_ABSOLUTE, // 绝对位置模式
    POS_MODE_RELATIVE  // 相对位置模式(将当前位置设置为 2047)
} FT_SERVO_POS_MODE;

typedef enum
{
    ONLINE,
    OFFLINE,
    PROTECTED
} FT_SERVO_STATUS;

typedef struct Class_FT_Servo
{
    uint8_t Servo_ID;
    FT_SERVO_POS_MODE Pos_Mode;
    FT_SERVO_STATUS Status;

    uint16_t Max_Pos; // 最大位置
    uint16_t Min_Pos; // 最小位置
    uint16_t Max_Spd; // 最大速度
    uint16_t Min_Spd; // 最小速度
    uint16_t Max_Acc; // 最大加速度
    uint16_t Min_Acc; // 最小加速度

    uint16_t Target_Pos; // 目标位置 0-4095 对应 0-360度
    uint16_t Target_Spd; // 速度 V=spd*0.732 rpm，最大spd设置为90
    uint16_t Target_Acc; // 加速度 A=acc*8.7 deg/s^2，最大acc为设置为250

    uint16_t Reset_Pos;

    uint16_t Pos;
    uint16_t Speed;
    uint16_t Load;
    uint16_t Voltage;
    uint16_t Temper;
    uint16_t Move;
    uint16_t Current;

    float Error; // PID循迹用
    pClass_PID PID;
    float PID_Output;

    bool is_inited;

    void (*Init)(struct Class_FT_Servo *this, FT_SERVO_POS_MODE Pos_Mode, uint16_t Max_Pos, uint16_t Min_Pos, uint16_t Max_Spd, uint16_t Min_Spd, uint16_t Max_Acc, uint16_t Min_Acc);
    void (*Set_Mid_Pos)(struct Class_FT_Servo *this);
    void (*Set_Min_Pos)(struct Class_FT_Servo *this, uint16_t pos);
    void (*Set_Max_Pos)(struct Class_FT_Servo *this, uint16_t pos);
    void (*Set_Min_Spd)(struct Class_FT_Servo *this, uint16_t spd);
    void (*Set_Max_Spd)(struct Class_FT_Servo *this, uint16_t spd);
    void (*Set_Min_Acc)(struct Class_FT_Servo *this, uint16_t acc);
    void (*Set_Max_Acc)(struct Class_FT_Servo *this, uint16_t acc);
    void (*Set_Target_Status)(struct Class_FT_Servo *this, uint16_t pos, uint16_t spd, uint16_t acc);
    bool (*Ping)(struct Class_FT_Servo *this);
    void (*FeedBack)(struct Class_FT_Servo *this);
    void (*Safety_Check)(struct Class_FT_Servo *this);
    void (*Update_PID)(struct Class_FT_Servo *this);

} Class_FT_Servo, *pClass_FT_Servo;

pClass_FT_Servo Create_FT_Servo(uint8_t ID);
pClass_FT_Servo Get_FT_Servo_Handle(uint8_t ID);

void FT_Servo_Init(pClass_FT_Servo this, FT_SERVO_POS_MODE Pos_Mode, uint16_t Max_Pos, uint16_t Min_Pos, uint16_t Max_Spd, uint16_t Min_Spd, uint16_t Max_Acc, uint16_t Min_Acc);
void FT_Servo_Set_Max_Pos(pClass_FT_Servo this, uint16_t pos);
void FT_Servo_Set_Min_Pos(pClass_FT_Servo this, uint16_t pos);
void FT_Servo_Set_Max_Spd(pClass_FT_Servo this, uint16_t spd);
void FT_Servo_Set_Min_Spd(pClass_FT_Servo this, uint16_t spd);
void FT_Servo_Set_Max_Acc(pClass_FT_Servo this, uint16_t acc);
void FT_Servo_Set_Min_Acc(pClass_FT_Servo this, uint16_t acc);
void FT_Servo_Set_Mid_Pos(pClass_FT_Servo this);
void FT_Servo_Set_Target_Status(pClass_FT_Servo this, uint16_t pos, uint16_t spd, uint16_t acc);
bool FT_Servo_Ping(pClass_FT_Servo this);
void FT_Servo_FeedBack(pClass_FT_Servo this);
void FT_Servo_Safety_Check(pClass_FT_Servo this);
void FT_Servo_Data_Process(pClass_UART this);
void FT_Servo_Update_PID(pClass_FT_Servo this);

void ftUart_Send(uint8_t *nDat, int nLen);
int ftUart_Read(uint8_t *nDat, int nLen);
void ftBus_Delay(void);
#endif