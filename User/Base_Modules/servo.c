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

    servo->Init = Servo_Init;
    servo->Set_Angle = Servo_Set_Angle;
    servo->angle_to_CCR = Servo_angle_to_CCR;

    return servo;
}
/**
 * @brief 舵机初始化
 *
 * @param self 舵机实例指针
 * @param angle 初始角度
 */
void Servo_Init(pClass_Servo this, GPTIMER_Regs *PWM_INST, uint32_t PWM_IDX, uint32_t ARR, float angle_max)
{
    this->PWM_INST = PWM_INST;
    this->PWM_IDX = PWM_IDX;
    this->ARR = ARR;
    this->angle_max = angle_max;

    DL_TimerG_setCaptureCompareValue(this->PWM_INST, 0, this->PWM_IDX);
}
/**
 * @brief 舵机输出
 *
 * @param self 舵机实例指针
 * @param angle 目标角度
 */
void Servo_Set_Angle(pClass_Servo this, float angle)
{
    if (angle >= 0 && angle <= this->angle_max)
    {
        DL_TimerG_setCaptureCompareValue(this->PWM_INST, this->angle_to_CCR(this, angle), this->PWM_IDX);
    }
}

/**
 * @brief 将角度转换为CCR
 *
 * @param self 舵机实例指针
 * @param angle 角度
 * @return uint32_t CCR
 */
uint32_t Servo_angle_to_CCR(pClass_Servo this, float angle)
{
    uint32_t CCR = 0;
    CCR = (uint32_t)((angle / this->angle_max) * this->ARR / 10.0f + this->ARR / 40.0f);
    return CCR;
}
