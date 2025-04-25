#include "Base_Modules/servo.h"

// <<<<<<<<<   实例声明     <<<<<<<<<<<
static Class_Servo _Servo_0 = {0};
static Class_Servo _Servo_1 = {0};
// >>>>>>>>>   实例声明     >>>>>>>>>>>

/**
 * @brief 创建一个舵机对象
 *
 * @param PWM_INST 时钟句柄
 * @param PWM_IDX PWM时钟通道
 * @return Class_Servo*
 */
pClass_Servo create_Servo(uint8_t index)
{
    pClass_Servo servo = NULL;

    switch (index)
    {
    case 0:
        servo = &_Servo_0;
        break;
    case 1:
        servo = &_Servo_1;
        break;
    default:
        break;
    }
    servo->PID = create_PID();
    servo->Init = Servo_Init;
    servo->Set_Angle = Servo_Set_Angle;
    servo->angle_to_CCR = Servo_angle_to_CCR;
    servo->Update_PID = Servo_Update_PID;

    return servo;
}

/**
 * @brief 获取舵机实例指针
 *
 * @param index 舵机索引
 * @return pClass_Servo
 */
pClass_Servo Get_Servo_INST(uint8_t index)
{
    switch (index)
    {
    case 0:
        return &_Servo_0;
    case 1:
        return &_Servo_1;
    default:
        return NULL;
    }
}

/**
 * @brief 舵机初始化
 *
 * @param this 舵机实例指针
 * @param angle 初始角度
 */
void Servo_Init(pClass_Servo this, GPTIMER_Regs *PWM_INST, uint32_t PWM_IDX, uint32_t ARR, float Angle_Max)
{
    this->PWM_INST = PWM_INST;
    this->PWM_IDX = PWM_IDX;
    this->ARR = ARR;
    this->Angle_Max = Angle_Max;

    this->PID->PID_Init(this->PID, 0.0f, 1.0f, 0.00f, 0.00f, this->Angle_Max / 2.0f, this->Angle_Max / 2.0f, PID_DELTA_T, 0.00f, 0.00f, 0.00f, 0.00f, PID_D_First_DISABLE);

    this->Set_Angle(this, 90.0f);

    this->is_inited = true;
}
/**
 * @brief 舵机输出
 *
 * @param this 舵机实例指针
 * @param angle 目标角度
 */
void Servo_Set_Angle(pClass_Servo this, float angle)
{
    if (angle >= 0 && angle <= this->Angle_Max)
    {
        DL_TimerG_setCaptureCompareValue(this->PWM_INST, this->angle_to_CCR(this, angle), this->PWM_IDX);
    }
}

/**
 * @brief 将角度转换为CCR
 *
 * @param this 舵机实例指针
 * @param angle 角度
 * @return uint32_t CCR
 */
uint32_t Servo_angle_to_CCR(pClass_Servo this, float angle)
{
    uint32_t CCR = 0;
    CCR = (uint32_t)((angle / this->Angle_Max) * this->ARR / 10.0f + this->ARR / 40.0f);
    return CCR;
}

/**
 * @brief PID控制回调函数
 *
 * @param this 舵机实例指针
 */
void Servo_Update_PID(pClass_Servo this)
{
    this->PID->Set_Target(this->PID, 0);
    this->PID->Set_Now(this->PID, this->Error);
    this->PID->TIM_Adjust_PeriodElapsedCallback(this->PID);

    float output = this->PID->Get_PID_Out(this->PID) + this->Angle_Max / 2.0f;
    this->Set_Angle(this, output);
}