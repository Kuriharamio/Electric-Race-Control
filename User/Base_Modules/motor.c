#include "Base_Modules/motor.h"

static Class_Motor _Motor_RB = {RIGHT_BACK};
static Class_Motor _Motor_LB = {LEFT_BACK};
static Class_Motor _Motor_RF = {RIGHT_FRONT};
static Class_Motor _Motor_LF = {LEFT_FRONT};

pClass_Motor create_motor(SIDE side)
{
    pClass_Motor tmp_motor_ptr = NULL;

    switch (side)
    {
    case LEFT_BACK:
        tmp_motor_ptr = &_Motor_LB;
        break;
    case LEFT_FRONT:
        tmp_motor_ptr = &_Motor_LF;
        break;
    case RIGHT_BACK:
        tmp_motor_ptr = &_Motor_RB;
        break;
    case RIGHT_FRONT:
        tmp_motor_ptr = &_Motor_RF;
        break;
    default:
        return NULL;
    }

    tmp_motor_ptr->Init = Motor_Init;

    tmp_motor_ptr->Configure_IN_1 = Motor_Configure_IN_1;
    tmp_motor_ptr->Configure_IN_2 = Motor_Configure_IN_2;
    tmp_motor_ptr->Configure_PWM = Motor_Configure_PWM;
    tmp_motor_ptr->Configure_ENCODER_A = Motor_Configure_ENCODER_A;
    tmp_motor_ptr->Configure_ENCODER_B = Motor_Configure_ENCODER_B;

    tmp_motor_ptr->TIM_PID_PeriodElapsedCallback = Motor_TIM_PID_PeriodElapsedCallback;
    tmp_motor_ptr->Encoder_Callback = Motor_Encoder_Callback;
    tmp_motor_ptr->Control = Motor_Control;
    tmp_motor_ptr->Output = Motor_Output;

    tmp_motor_ptr->PID_Speed = create_PID();

    return tmp_motor_ptr;
}

/**
 * @brief 电机初始化
 *
 */
void Motor_Init(pClass_Motor this, float __radius, float __Output_Max, float __Speed_Max, int __Gearbox_Rate, int __Per_Pulse, int __Frequency_doubling)
{
    this->Direction = BRAKE;

    this->Radius = __radius;
    this->Output_Max = __Output_Max;
    this->Speed_Max = __Speed_Max;
    this->Encoder_Num_Per_Round = __Gearbox_Rate * __Per_Pulse * __Frequency_doubling;

    this->Total_Encoder_Tick = 0;
    this->Last_Encoder_Tick = 0;
    this->Now_Speed = 0.0f;
    this->Target_Speed = 0.0f;

    this->Output_Now = 0;
    this->is_inited = true;
}

/**
 * @brief 配置IN1引脚
 *
 * @param __IN_1_PORT IN1引脚端口
 * @param __IN_1_PIN IN1引脚引脚号
 */
void Motor_Configure_IN_1(pClass_Motor this, GPIO_Regs *__IN_1_PORT, uint32_t __IN_1_PIN)
{
    this->IN_1_PORT = __IN_1_PORT;
    this->IN_1_PIN = __IN_1_PIN;
}
/**
 * @brief 配置IN2引脚
 *
 * @param __IN_2_PORT IN2引脚端口
 * @param __IN_2_PIN IN2引脚引脚号
 */
void Motor_Configure_IN_2(pClass_Motor this, GPIO_Regs *__IN_2_PORT, uint32_t __IN_2_PIN)
{
    this->IN_2_PORT = __IN_2_PORT;
    this->IN_2_PIN = __IN_2_PIN;
}

/**
 * @brief 配置PWM引脚
 *
 * @param __PWM_INST PWM引脚端口
 * @param __PWM_IDX PWM引脚引脚号
 */
void Motor_Configure_PWM(pClass_Motor this, GPTIMER_Regs *__PWM_INST, uint32_t __PWM_IDX)
{
    this->PWM_INST = __PWM_INST;
    this->PWM_IDX = __PWM_IDX;
}

/**
 * @brief 配置编码器A引脚
 *
 * @param __ENCODER_A_PORT 编码器A引脚端口
 * @param __ENCODER_A_PIN 编码器A引脚引脚号
 * @note 要求AB同一个端口，否则需要修改此函数代码以及中断函数
 */
void Motor_Configure_ENCODER_A(pClass_Motor this, GPIO_Regs *__ENCODER_A_PORT, uint32_t __ENCODER_A_PIN)
{
    this->ENCODER_PORT = __ENCODER_A_PORT;
    this->ENCODER_A_PORT = __ENCODER_A_PORT;
    this->ENCODER_A_PIN = __ENCODER_A_PIN;
}

/**
 * @brief 配置编码器B引脚
 *
 * @param __ENCODER_B_PORT 编码器B引脚端口
 * @param __ENCODER_B_PIN 编码器B引脚引脚号
 * @note 要求AB同一个端口，否则需要修改此函数代码以及中断函数
 */
void Motor_Configure_ENCODER_B(pClass_Motor this, GPIO_Regs *__ENCODER_B_PORT, uint32_t __ENCODER_B_PIN)
{
    this->ENCODER_PORT = __ENCODER_B_PORT;
    this->ENCODER_B_PORT = __ENCODER_B_PORT;
    this->ENCODER_B_PIN = __ENCODER_B_PIN;
}

/**
 * @brief 电机控制
 *
 * @param __DIR 电机方向
 */
void Motor_Control(pClass_Motor this)
{
    switch (this->Direction)
    {
    case FORWARD: // IN_1 = 1, IN_2 = 0
        DL_GPIO_setPins(this->IN_1_PORT, this->IN_1_PIN);
        DL_GPIO_clearPins(this->IN_2_PORT, this->IN_2_PIN);
        DL_TimerG_setCaptureCompareValue(this->PWM_INST, (uint32_t)(fabs(this->Output_Now)), this->PWM_IDX);
        break;
    case BACKWARD: // IN_1 = 0, IN_2 = 1
        DL_GPIO_setPins(this->IN_2_PORT, this->IN_2_PIN);
        DL_GPIO_clearPins(this->IN_1_PORT, this->IN_1_PIN);
        DL_TimerG_setCaptureCompareValue(this->PWM_INST, (uint32_t)(fabs(this->Output_Now)), this->PWM_IDX);
        break;
    case BRAKE:
        DL_GPIO_setPins(this->IN_1_PORT, this->IN_1_PIN);
        DL_GPIO_setPins(this->IN_2_PORT, this->IN_2_PIN);
        DL_TimerG_setCaptureCompareValue(this->PWM_INST, 0, this->PWM_IDX);
        break;
    default:
        break;
    }
}

/**
 * @brief 输出PWM
 *
 */
void Motor_Output(pClass_Motor this)
{
    if (this->Output_Now > this->Output_Max)
        this->Output_Now = this->Output_Max;
    if (this->Output_Now < -this->Output_Max)
        this->Output_Now = -this->Output_Max;

    if (fabs(this->Target_Speed) <= 0.0025)
        this->Output_Now = 0;

    if (this->Output_Now < -0.1)
    {
        this->Direction = BACKWARD;
    }
    else if (this->Output_Now > 0.1)
    {
        this->Direction = FORWARD;
    }
    else
    {
        this->Direction = BRAKE;
    }

    this->Control(this);
}

/**
 * @brief 编码器计算回调函数
 *
 * @param phase 编码器相位
 */
void Motor_Encoder_Callback(pClass_Motor this, char phase)
{
    switch (phase)
    {
    case 'A':
    {
        if (DL_GPIO_readPins(this->ENCODER_A_PORT, this->ENCODER_A_PIN))
        {
            if (DL_GPIO_readPins(this->ENCODER_B_PORT, this->ENCODER_B_PIN))
            {
                this->Total_Encoder_Tick++;
            }
            else
            {
                this->Total_Encoder_Tick--;
            }
        }
        else
        {
            if (DL_GPIO_readPins(this->ENCODER_B_PORT, this->ENCODER_B_PIN))
            {
                this->Total_Encoder_Tick--;
            }
            else
            {
                this->Total_Encoder_Tick++;
            }
        }
    }
    break;
    case 'B':
    {
        if (DL_GPIO_readPins(this->ENCODER_B_PORT, this->ENCODER_B_PIN))
        {
            if (DL_GPIO_readPins(this->ENCODER_A_PORT, this->ENCODER_A_PIN))
            {
                this->Total_Encoder_Tick--;
            }
            else
            {
                this->Total_Encoder_Tick++;
            }
        }
        else
        {
            if (DL_GPIO_readPins(this->ENCODER_A_PORT, this->ENCODER_A_PIN))
            {
                this->Total_Encoder_Tick++;
            }
            else
            {
                this->Total_Encoder_Tick--;
            }
        }
    }
    break;
    }
}

/**
 * @brief TIM定时器中断计算回调函数
 *
 */
void Motor_TIM_PID_PeriodElapsedCallback(pClass_Motor this)
{
    if (this->Target_Speed > this->Speed_Max)
    {
        this->Target_Speed = this->Speed_Max;
    }
    if (this->Target_Speed < -this->Speed_Max)
    {
        this->Target_Speed = -this->Speed_Max;
    }

    this->PID_Speed->Set_Target((this->PID_Speed), this->Target_Speed);
    this->PID_Speed->Set_Now((this->PID_Speed), this->Now_Speed);
    this->PID_Speed->TIM_Adjust_PeriodElapsedCallback((this->PID_Speed));

    this->Output_Now = (this->PID_Speed->Get_PID_Out((this->PID_Speed)));

    this->Output(this);
}

void PID_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(PID_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        if(_Motor_LB.is_inited)
            _Motor_LB.TIM_PID_PeriodElapsedCallback(&_Motor_LB);
        if(_Motor_LF.is_inited)
            _Motor_LF.TIM_PID_PeriodElapsedCallback(&_Motor_LF);
        if(_Motor_RB.is_inited)
            _Motor_RB.TIM_PID_PeriodElapsedCallback(&_Motor_RB);
        if(_Motor_RF.is_inited)
            _Motor_RF.TIM_PID_PeriodElapsedCallback(&_Motor_RF);
        break;

    default:
        break;
    }
}

// 里程计中断服务函数
void ENCODER_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(ENCODER_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        // 计算速度
        if(_Motor_LB.is_inited){
            _Motor_LB.Now_Speed = (float)(_Motor_LB.Total_Encoder_Tick) / (float)(_Motor_LB.Encoder_Num_Per_Round) / 0.005 * 2 * PI * _Motor_LB.Radius;
            _Motor_LB.Total_Encoder_Tick = 0;
        }
        if(_Motor_LF.is_inited){
            _Motor_LF.Now_Speed = (float)(_Motor_LF.Total_Encoder_Tick) / (float)(_Motor_LF.Encoder_Num_Per_Round) / 0.005 * 2 * PI * _Motor_LF.Radius;
            _Motor_LF.Total_Encoder_Tick = 0;
        }
        if(_Motor_RB.is_inited){
            _Motor_RB.Now_Speed = (float)(_Motor_RB.Total_Encoder_Tick) / (float)(_Motor_RB.Encoder_Num_Per_Round) / 0.005 * 2 * PI * _Motor_RB.Radius;
            _Motor_RB.Total_Encoder_Tick = 0;
        }
        if(_Motor_RF.is_inited){
            _Motor_RF.Now_Speed = (float)(_Motor_RF.Total_Encoder_Tick) / (float)(_Motor_RF.Encoder_Num_Per_Round) / 0.005 * 2 * PI * _Motor_RF.Radius;
            _Motor_RF.Total_Encoder_Tick = 0;
        }

        // 更新里程计

        break;

    default:
        break;
    }
}

void GROUP1_IRQHandler(void)
{

    /*******************************
                编码器中断
    *******************************/
    // 左后轮
    uint32_t EN_LB = DL_GPIO_getEnabledInterruptStatus(_Motor_LB.ENCODER_PORT, _Motor_LB.ENCODER_A_PIN | _Motor_LB.ENCODER_B_PIN);

    if ((EN_LB & _Motor_LB.ENCODER_A_PIN) == _Motor_LB.ENCODER_A_PIN)
    {
        _Motor_LB.Encoder_Callback(&(_Motor_LB), 'A');
        DL_GPIO_clearInterruptStatus(_Motor_LB.ENCODER_PORT, _Motor_LB.ENCODER_A_PIN);
    }

    if ((EN_LB & _Motor_LB.ENCODER_B_PIN) == _Motor_LB.ENCODER_B_PIN)
    {
        _Motor_LB.Encoder_Callback(&(_Motor_LB), 'B');
        DL_GPIO_clearInterruptStatus(_Motor_LB.ENCODER_PORT, _Motor_LB.ENCODER_B_PIN);
    }

    // 右后轮
    uint32_t EN_RB = DL_GPIO_getEnabledInterruptStatus(_Motor_RB.ENCODER_PORT, _Motor_RB.ENCODER_A_PIN | _Motor_RB.ENCODER_B_PIN);

    if ((EN_RB & _Motor_RB.ENCODER_A_PIN) == _Motor_RB.ENCODER_A_PIN)
    {
        _Motor_RB.Encoder_Callback(&(_Motor_RB), 'A');
        DL_GPIO_clearInterruptStatus(_Motor_RB.ENCODER_PORT, _Motor_RB.ENCODER_A_PIN);
    }

    if ((EN_RB & _Motor_RB.ENCODER_B_PIN) == _Motor_RB.ENCODER_B_PIN)
    {
        _Motor_RB.Encoder_Callback(&(_Motor_RB), 'B');
        DL_GPIO_clearInterruptStatus(_Motor_RB.ENCODER_PORT, _Motor_RB.ENCODER_B_PIN);
    }

    // 左前轮
    uint32_t EN_LF = DL_GPIO_getEnabledInterruptStatus(_Motor_LF.ENCODER_PORT, _Motor_LF.ENCODER_A_PIN | _Motor_LF.ENCODER_B_PIN);

    if ((EN_LF & _Motor_LF.ENCODER_A_PIN) == _Motor_LF.ENCODER_A_PIN)
    {
        _Motor_LF.Encoder_Callback(&(_Motor_LF), 'A');
        DL_GPIO_clearInterruptStatus(_Motor_LF.ENCODER_PORT, _Motor_LF.ENCODER_A_PIN);
    }

    if ((EN_LF & _Motor_LF.ENCODER_B_PIN) == _Motor_LF.ENCODER_B_PIN)
    {
        _Motor_LF.Encoder_Callback(&(_Motor_LF), 'B');
        DL_GPIO_clearInterruptStatus(_Motor_LF.ENCODER_PORT, _Motor_LF.ENCODER_B_PIN);
    }

    // 右前轮
    uint32_t EN_RF = DL_GPIO_getEnabledInterruptStatus(_Motor_RF.ENCODER_PORT, _Motor_RF.ENCODER_A_PIN | _Motor_RF.ENCODER_B_PIN);

    if ((EN_RF & _Motor_RF.ENCODER_A_PIN) == _Motor_RF.ENCODER_A_PIN)
    {
        _Motor_RF.Encoder_Callback(&(_Motor_RF), 'A');
        DL_GPIO_clearInterruptStatus(_Motor_RF.ENCODER_PORT, _Motor_RF.ENCODER_A_PIN);
    }

    if ((EN_RF & _Motor_RF.ENCODER_B_PIN) == _Motor_RF.ENCODER_B_PIN)
    {
        _Motor_RF.Encoder_Callback(&(_Motor_RF), 'B');
        DL_GPIO_clearInterruptStatus(_Motor_RF.ENCODER_PORT, _Motor_RF.ENCODER_B_PIN);
    }
}
