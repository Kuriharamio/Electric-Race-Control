#include "Base_Modules/ptz.h"
#ifdef USE_PTZ
// <<<<<<<<<   实例声明     <<<<<<<<<<<
Class_PTZ _PTZ = {0};
// >>>>>>>>>   实例声明     >>>>>>>>>>>

pClass_PTZ Create_PTZ(void)
{
    pClass_PTZ temp = &_PTZ;
    temp->Init = PTZ_Init;
    temp->Update = PTZ_Update;
    temp->FT_Servo_Controller = Create_UART(FT_SERVO_UART_INDEX);
    temp->Servo_Down = Create_FT_Servo(SERVO_DOWN_INDEX);
    temp->Servo_Up = Create_FT_Servo(SERVO_UP_INDEX);
    return temp;
}

pClass_PTZ Get_PTZ_INST(void)
{
    return &_PTZ;
}

void PTZ_Init(pClass_PTZ this)
{
    this->FT_Servo_Controller->Init(this->FT_Servo_Controller, FT_SERVO_RX_LEN_MAX, 1);
    this->FT_Servo_Controller->Configure_Mode(this->FT_Servo_Controller, DEBUG_STRING);
    this->FT_Servo_Controller->Configure_Callback(this->FT_Servo_Controller, FT_Servo_Data_Process);

    this->Servo_Down->Init(this->Servo_Down, POS_MODE_ABSOLUTE, 1800, 2200, 0, 60, 0, 50);
    this->Servo_Up->Init(this->Servo_Up, POS_MODE_ABSOLUTE, 1800, 2200, 0, 60, 0, 50);

    this->is_inited = true;
}

void PTZ_Update(pClass_PTZ this)
{
    static uint8_t step = 0;
    step++;

    switch (step)
    {
    case 1:
        if (this->Servo_Down->Status == ONLINE || this->Servo_Down->Status == PROTECTED)
        {
            this->Servo_Down->FeedBack(this->Servo_Down); // 更新舵机反馈信息
            this->Servo_Down->Safety_Check(this->Servo_Down);
            if (this->Servo_Down->Status == PROTECTED)
            {
                this->Servo_Down->Set_Target_Status(this->Servo_Down, this->Servo_Down->Reset_Pos, 30, 50);
            }
            else
            {

                // if (Get_FT_Servo_Handle(SERVO_DOWN_INDEX)->is_inited)
                // {
                //     Get_FT_Servo_Handle(SERVO_DOWN_INDEX)->Update_PID(Get_FT_Servo_Handle(SERVO_DOWN_INDEX));
                // }
            }

            this->ID[0] = this->Servo_Down->Servo_ID;
            this->Position[0] = this->Servo_Down->Target_Pos;
            this->Speed[0] = this->Servo_Down->Target_Spd;
            this->ACC[0] = this->Servo_Down->Target_Acc;
        }
        else
        {
            this->ID[0] = 0; // 舵机离线
            this->Position[0] = 0;
            this->Speed[0] = 0;
            this->ACC[0] = 0;
        }
        break;
    case 2:
        if (this->Servo_Up->Status == ONLINE || this->Servo_Up->Status == PROTECTED)
        {
            this->Servo_Up->FeedBack(this->Servo_Up); // 更新舵机反馈信息
            this->Servo_Up->Safety_Check(this->Servo_Up);
            if (this->Servo_Up->Status == PROTECTED)
            {
                this->Servo_Up->Set_Target_Status(this->Servo_Up, this->Servo_Up->Reset_Pos, 30, 50);
            }
            else
            {

                // if (Get_FT_Servo_Handle(SERVO_UP_INDEX)->is_inited)
                // {
                //     Get_FT_Servo_Handle(SERVO_UP_INDEX)->Update_PID(Get_FT_Servo_Handle(SERVO_UP_INDEX));
                // }
            }

            this->ID[1] = this->Servo_Up->Servo_ID;
            this->Position[1] = this->Servo_Up->Target_Pos;
            this->Speed[1] = this->Servo_Up->Target_Spd;
            this->ACC[1] = this->Servo_Up->Target_Acc;
        }
        else
        {
            this->ID[1] = 0; // 舵机离线
            this->Position[1] = 0;
            this->Speed[1] = 0;
            this->ACC[1] = 0;
        }
        break;
    case 3:
        SyncWritePosEx(this->ID, 2, this->Position, this->Speed, this->ACC);
        step = 0;
        break;
    default:
        break;
    }
}

#endif