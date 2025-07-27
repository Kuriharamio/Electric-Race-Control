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
    temp->Draw_Img = PTZ_Draw_Img;
    temp->Draw_Func = PTZ_Draw_Func;
    temp->Draw_Num = PTZ_Draw_Num;
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

    this->Servo_Down->Init(this->Servo_Down, POS_MODE_ABSOLUTE, 2190, 1890, 0, 60, 0, 50);
    this->Servo_Up->Init(this->Servo_Up, POS_MODE_ABSOLUTE, 2150, 1900, 0, 60, 0, 50);

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
                // this->Servo_Down->Update_PID(this->Servo_Down);
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

                // this->Servo_Up->Update_PID(this->Servo_Up);
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

// void PTZ_Update(pClass_PTZ this)
// {

//     this->Servo_Down->Update_PID(this->Servo_Down);
//     this->ID[0] = this->Servo_Down->Servo_ID;
//     this->Position[0] = this->Servo_Down->Target_Pos;
//     this->Speed[0] = this->Servo_Down->Target_Spd;
//     this->ACC[0] = this->Servo_Down->Target_Acc;

//     this->Servo_Up->Update_PID(this->Servo_Up);

//     this->ID[1] = this->Servo_Up->Servo_ID;
//     this->Position[1] = this->Servo_Up->Target_Pos;
//     this->Speed[1] = this->Servo_Up->Target_Spd;
//     this->ACC[1] = this->Servo_Up->Target_Acc;

//     SyncWritePosEx(this->ID, 2, this->Position, this->Speed, this->ACC);

// }

uint16_t map(uint16_t x, uint16_t min, uint16_t max){
    return (uint16_t)(min + x * (float)(max - min) / 4095);
}

#include "PTZ_h/subfunc.h"
void PTZ_Draw_Func(pClass_PTZ this)
{
    uint16_t spd = 40;
    if (func_point_cnt == 0)
        return;

    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    uint16_t x = map(func_path[0][0], this->Servo_Down->Min_Pos, this->Servo_Down->Max_Pos);
    uint16_t y = map(func_path[0][1], this->Servo_Up->Min_Pos, this->Servo_Up->Max_Pos);

    this->Servo_Down->Set_Target_Status(this->Servo_Down, x, spd, 10);
    this->Servo_Up->Set_Target_Status(this->Servo_Up, y, spd, 10);
    last_x = x;
    last_y = y;
    while (abs(this->Servo_Down->Pos - x) > 1 || abs(this->Servo_Up->Pos - y) > 1)
    {
    }


    LAZER(ON);
    for (uint16_t i = 1; i < func_point_cnt; ++i)
    {
        x = map(func_path[i][0], this->Servo_Down->Min_Pos, this->Servo_Down->Max_Pos);
        y = map(func_path[i][1], this->Servo_Up->Min_Pos, this->Servo_Up->Max_Pos);
        if (abs(last_x - x) < 3 && abs(last_y - y) < 3)
        {
            continue;
        }

        this->Servo_Down->Set_Target_Status(this->Servo_Down, x, spd, 10);
        this->Servo_Up->Set_Target_Status(this->Servo_Up, y, spd, 10);
        last_x = x;
        last_y = y;
        // while (abs(this->Servo_Down->Pos - x) > 2 || abs(this->Servo_Up->Pos - y) > 2)
        // {
        // }
        delay_ms(5);
    }
    LAZER(OFF);

    this->Servo_Down->Set_Target_Status(this->Servo_Down, 0.5 * (this->Servo_Down->Max_Pos + this->Servo_Down->Min_Pos), spd, 10);
    this->Servo_Up->Set_Target_Status(this->Servo_Up, 0.5 * (this->Servo_Up->Max_Pos + this->Servo_Up->Min_Pos), spd, 10);
}

#include "PTZ_h/subpaths.h"
void PTZ_Draw_Img(pClass_PTZ this)
{
    uint16_t spd = 60;
    uint16_t acc = 40;

    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    if (subpath_cnt == 0)
        return;

    const int16_t (*p)[2] = subpath_points;
    LAZER(OFF);

    for (uint16_t i = 0; i < subpath_cnt; ++i)
    {
        uint16_t x = map(p[0][0], this->Servo_Down->Min_Pos, this->Servo_Down->Max_Pos);
        uint16_t y = map(p[0][1], this->Servo_Up->Min_Pos, this->Servo_Up->Max_Pos);
        uint16_t len = subpath_lens[i];

        this->Servo_Down->Set_Target_Status(this->Servo_Down, x, spd, acc);
        this->Servo_Up->Set_Target_Status(this->Servo_Up, y, spd, acc);
        last_x = x;
        last_y = y;
        while (abs(this->Servo_Down->Pos - x) > 1 || abs(this->Servo_Up->Pos - y) > 1)
        {
        }

        LAZER(ON);
        for (uint16_t k = 1; k < len; ++k)
        {

            if (k == len - 1)
            {
                delay_ms(100);
            }
            uint16_t x = map(p[k][0], this->Servo_Down->Min_Pos, this->Servo_Down->Max_Pos);
            uint16_t y = map(p[k][1], this->Servo_Up->Min_Pos, this->Servo_Up->Max_Pos);
            if (abs(x - last_x) < 3 && abs(y - last_y) < 3)
            {
                continue;
            }
            this->Servo_Down->Set_Target_Status(this->Servo_Down, x, spd, acc);
            this->Servo_Up->Set_Target_Status(this->Servo_Up, y, spd, acc);
            last_x = x;
            last_y = y;
            // while(abs(this->Servo_Down->Pos - x) > 2 || abs(this->Servo_Up->Pos - y) > 2)
            // {

            // }
            delay_ms(15);
        }
        LAZER(OFF);

        p += len;
    }

    this->Servo_Down->Set_Target_Status(this->Servo_Down, 0.5 * (this->Servo_Down->Max_Pos + this->Servo_Down->Min_Pos), spd, acc);
    this->Servo_Up->Set_Target_Status(this->Servo_Up, 0.5 * (this->Servo_Up->Max_Pos + this->Servo_Up->Min_Pos), spd, acc);
}


#include "PTZ_h/path0.h"
#include "PTZ_h/path1.h"
#include "PTZ_h/path2.h"
#include "PTZ_h/path3.h"
#include "PTZ_h/path4.h"
#include "PTZ_h/path5.h"
#include "PTZ_h/path6.h"
#include "PTZ_h/path7.h"
#include "PTZ_h/path8.h"
#include "PTZ_h/path9.h"
#include "PTZ_h/pathdot.h"

void PTZ_Draw_Num(pClass_PTZ this, double num, bool is_float, char *pos)
{
    char buf[16];
    if (is_float)
        snprintf(buf, sizeof(buf), pos, num);
    else
        snprintf(buf, sizeof(buf), "%d", (int)num);
        
    uint8_t len = strlen(buf);
    uint16_t gap = 0;
    uint16_t width = 0;
    uint16_t width_dot = 0;
    if (is_float){
        width = (uint16_t)((float)(this->Servo_Down->Max_Pos - this->Servo_Down->Min_Pos) / (float)(strlen(buf) - 1));
        width_dot = (uint16_t)((float)(width) / 15.0f);
        width = (uint16_t)((float)(this->Servo_Down->Max_Pos - this->Servo_Down->Min_Pos - width_dot) / (float)(strlen(buf) - 1));
    }
    else{
        width = (uint16_t)((float)(this->Servo_Down->Max_Pos - this->Servo_Down->Min_Pos) / (float)(strlen(buf)));
    }
    gap = (uint32_t)((float)width / 10.0);
    // uint16_t height = this->Servo_Up->Max_Pos - this->Servo_Up->Min_Pos;
    uint16_t height = (uint16_t)(width * 2);

    uint16_t spd = 60;
    uint16_t acc = 40;

    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    uint16_t start_x = this->Servo_Down->Min_Pos;
    uint16_t start_y = this->Servo_Up->Min_Pos;

    for (char *p = buf; *p; ++p)
    {
        uint8_t idx;
        if (*p == '.')
            idx = 10; 
        else
            idx = *p - '0';

        uint16_t subpath_cnt_idx = 0;
        const uint16_t *subpath_lens_idx = NULL;
        const int16_t (*subpath_points_idx)[2] = NULL;

        switch(idx)
        {
            case 0:
                subpath_cnt_idx = subpath_cnt_0;
                subpath_lens_idx = subpath_lens_0;
                subpath_points_idx = subpath_points_0;
                break;
            case 1:
                subpath_cnt_idx = subpath_cnt_1;
                subpath_lens_idx = subpath_lens_1;
                subpath_points_idx = subpath_points_1;
                break;
            case 2:
                subpath_cnt_idx = subpath_cnt_2;
                subpath_lens_idx = subpath_lens_2;
                subpath_points_idx = subpath_points_2;
                break;
            case 3:
                subpath_cnt_idx = subpath_cnt_3;
                subpath_lens_idx = subpath_lens_3;
                subpath_points_idx = subpath_points_3;
                break;
            case 4:
                subpath_cnt_idx = subpath_cnt_4;
                subpath_lens_idx = subpath_lens_4;
                subpath_points_idx = subpath_points_4;
                break;
            case 5:
                subpath_cnt_idx = subpath_cnt_5;
                subpath_lens_idx = subpath_lens_5;
                subpath_points_idx = subpath_points_5;
                break;
            case 6:
                subpath_cnt_idx = subpath_cnt_6;
                subpath_lens_idx = subpath_lens_6;
                subpath_points_idx = subpath_points_6;
                break;
            case 7:
                subpath_cnt_idx = subpath_cnt_7;
                subpath_lens_idx = subpath_lens_7;
                subpath_points_idx = subpath_points_7;
                break;
            case 8:
                subpath_cnt_idx = subpath_cnt_8;
                subpath_lens_idx = subpath_lens_8;
                subpath_points_idx = subpath_points_8;
                break;
            case 9:
                subpath_cnt_idx = subpath_cnt_9;
                subpath_lens_idx = subpath_lens_9;
                subpath_points_idx = subpath_points_9;
                break;
            case 10:
                subpath_cnt_idx = subpath_cnt_dot;
                subpath_lens_idx = subpath_lens_dot;
                subpath_points_idx = subpath_points_dot;
                break;
            default:
                return;
        }


        if (subpath_cnt_idx == 0)
            return;

        const int16_t (*p)[2] = subpath_points_idx;
        LAZER(OFF);

        for (uint16_t i = 0; i < subpath_cnt_idx; ++i)
        {
            uint16_t x = map(p[0][0], start_x, start_x + width);
            if (idx == 10)
                x = map(p[0][0], start_x, start_x + width_dot);
            uint16_t y = map(p[0][1], start_y, start_y + height);
            uint16_t len = subpath_lens_idx[i];

            this->Servo_Down->Set_Target_Status(this->Servo_Down, x, spd, acc);
            this->Servo_Up->Set_Target_Status(this->Servo_Up, y, spd, acc);
            last_x = x;
            last_y = y;
            while (abs(this->Servo_Down->Pos - x) > 1 || abs(this->Servo_Up->Pos - y) > 1)
            {
            }

            LAZER(ON);
            for (uint16_t k = 1; k < len; ++k)
            {

                if (k == len - 1)
                {
                    delay_ms(100);
                }
                uint16_t x = map(p[k][0], start_x, start_x + width);
                if (idx == 10)
                    x = map(p[0][0], start_x, start_x + width_dot);
                uint16_t y = map(p[k][1], start_y, start_y + height);
                // if (abs(x - last_x) < 3 && abs(y - last_y) < 3)
                // {
                //     continue;
                // }
                this->Servo_Down->Set_Target_Status(this->Servo_Down, x, spd, acc);
                this->Servo_Up->Set_Target_Status(this->Servo_Up, y, spd, acc);
                last_x = x;
                last_y = y;
                // while(abs(this->Servo_Down->Pos - x) > 2 || abs(this->Servo_Up->Pos - y) > 2)
                // {

                // }
                delay_ms(20);
            }
            LAZER(OFF);

            p += len;
        }
        if (idx == 10)
            start_x += (width_dot + gap);
        else
            start_x += (width + gap);
    }

    this->Servo_Down->Set_Target_Status(this->Servo_Down, 0.5 * (this->Servo_Down->Max_Pos + this->Servo_Down->Min_Pos), spd, acc);
    this->Servo_Up->Set_Target_Status(this->Servo_Up, 0.5 * (this->Servo_Up->Max_Pos + this->Servo_Up->Min_Pos), spd, acc);
}
void PTZ_Draw_Rect(pClass_PTZ this)
{
    LAZER(OFF);
    this->Servo_Down->Set_Target_Status(this->Servo_Down, this->Servo_Down->Min_Pos, 20, 10);
    this->Servo_Up->Set_Target_Status(this->Servo_Up, this->Servo_Up->Min_Pos, 20, 10);
    delay_ms(1000);
    LAZER(ON);
    for (uint16_t i = this->Servo_Down->Min_Pos; i < this->Servo_Down->Max_Pos; i++)
    {
        this->Servo_Down->Set_Target_Status(this->Servo_Down, i, 20, 10);
        delay_ms(10);
    }
    for (uint16_t i = this->Servo_Up->Min_Pos; i < this->Servo_Up->Max_Pos; i++)
    {
        this->Servo_Up->Set_Target_Status(this->Servo_Up, i, 20, 10);
        delay_ms(10);
    }
    for (uint16_t i = this->Servo_Down->Max_Pos; i > this->Servo_Down->Min_Pos; i--)
    {
        this->Servo_Down->Set_Target_Status(this->Servo_Down, i, 20, 10);
        delay_ms(10);
    }
    for (uint16_t i = this->Servo_Up->Max_Pos; i > this->Servo_Up->Min_Pos; i--)
    {
        this->Servo_Up->Set_Target_Status(this->Servo_Up, i, 20, 10);
        delay_ms(10);
    }
    LAZER(OFF);
    this->Servo_Down->Set_Target_Status(this->Servo_Down, 0.5 * (this->Servo_Down->Max_Pos + this->Servo_Down->Min_Pos), 20, 10);
    this->Servo_Up->Set_Target_Status(this->Servo_Up, 0.5 * (this->Servo_Up->Max_Pos + this->Servo_Up->Min_Pos), 20, 10);
    delay_ms(1000);
}

#endif