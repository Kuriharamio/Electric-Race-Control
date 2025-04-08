#include "Base_Modules/motor.h"

Class_Motor *create_motor(GPIO_Regs *__SLP_PORT, uint32_t __SLP_PIN, GPTIMER_Regs *__PWM_INST, uint32_t __PWM_IDX, GPIO_Regs *__DIR_PORT, uint32_t __DIR_PIN, GPIO_Regs *__ENCODER_A_PORT, uint32_t __ENCODER_A_PIN, GPIO_Regs *__ENCODER_B_PORT, uint32_t __ENCODER_B_PIN)
{
    Class_Motor *tmp_motor_ptr = NULL;

    tmp_motor_ptr = (Class_Motor *)malloc(sizeof(Class_Motor));

    tmp_motor_ptr->Init = Motor_Init;
    
    tmp_motor_ptr->TIM_PID_PeriodElapsedCallback = Motor_TIM_PID_PeriodElapsedCallback;
    tmp_motor_ptr->Encoder_Callback = Motor_Encoder_Callback;
    tmp_motor_ptr->Control = Motor_Control;
    tmp_motor_ptr->Output = Motor_Output;

    tmp_motor_ptr->PID_Speed = create_PID();

    tmp_motor_ptr->SLP_PORT = __SLP_PORT;
    tmp_motor_ptr->SLP_PIN = __SLP_PIN;
    tmp_motor_ptr->PWM_INST = __PWM_INST;
    tmp_motor_ptr->PWM_IDX = __PWM_IDX;
    tmp_motor_ptr->DIR_PORT = __DIR_PORT;
    tmp_motor_ptr->DIR_PIN = __DIR_PIN;
    tmp_motor_ptr->ENCODER_A_PORT = __ENCODER_A_PORT;
    tmp_motor_ptr->ENCODER_A_PIN = __ENCODER_A_PIN;
    tmp_motor_ptr->ENCODER_B_PORT = __ENCODER_B_PORT;
    tmp_motor_ptr->ENCODER_B_PIN = __ENCODER_B_PIN;

    return tmp_motor_ptr;
}

/**
 * @brief 电机初始化
 *
 */
void Motor_Init(Class_Motor *self,double __side, double __radius, double __Output_Max, double __Speed_Max, int __Gearbox_Rate, int __Per_Pulse, int __Frequency_doubling)
{   
    self->Side = __side;

    self->Radius = __radius;
    self->Output_Max = __Output_Max;
    self->Speed_Max = __Speed_Max;
    self->Encoder_Num_Per_Round = __Gearbox_Rate * __Per_Pulse * __Frequency_doubling;

    self->Total_Encoder_Tick = 0;
    self->Last_Encoder_Tick = 0;
    self->Now_Speed = 0.0f;
    self->Target_Speed = 0.0f;

    self->Output_Now = 0;

    DL_GPIO_setPins(self->DIR_PORT, self->DIR_PIN);
    // DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
    // DL_TimerG_setCaptureCompareValue(self->PWM_INST, 39999, self->PWM_IDX);
}

/**
 * @brief 电机控制
 * 
 * @param __DIR 电机方向
 */
void Motor_Control(Class_Motor *self, Motor_Dir __DIR)
{
    if(self->Side == LEFT_BACK || self->Side == LEFT_FRONT){
        switch (__DIR)
        {
        case FORWARD: // nSLEEP = 1, DIR = 1
            DL_GPIO_clearPins(self->DIR_PORT, self->DIR_PIN);
            DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
            DL_TimerG_setCaptureCompareValue(self->PWM_INST, (uint32_t)(39999 - fabs(self->Output_Now)), self->PWM_IDX);
            break;
        case BACKWARD: // nSLEEP = 1, DIR = 0
            DL_GPIO_setPins(self->DIR_PORT, self->DIR_PIN);
            DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
            DL_TimerG_setCaptureCompareValue(self->PWM_INST, (uint32_t)(fabs(self->Output_Now)), self->PWM_IDX);
            break;
        case BRAKE:
            // DL_GPIO_clearPins(self->SLP_PORT, self->SLP_PIN);
            DL_GPIO_setPins(self->DIR_PORT, self->DIR_PIN);
            DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
            DL_TimerG_setCaptureCompareValue(self->PWM_INST, 0, self->PWM_IDX);
            break;
        default:
            break;
        }
    }else{
        switch (__DIR)
        {
        case FORWARD: // nSLEEP = 1, DIR = 1
            DL_GPIO_clearPins(self->DIR_PORT, self->DIR_PIN);
            DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
            DL_TimerG_setCaptureCompareValue(self->PWM_INST, (uint32_t)(fabs(self->Output_Now)), self->PWM_IDX);
            break;
        case BACKWARD: // nSLEEP = 1, DIR = 0
            DL_GPIO_setPins(self->DIR_PORT, self->DIR_PIN);
            DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
            DL_TimerG_setCaptureCompareValue(self->PWM_INST, (uint32_t)(39999 - fabs(self->Output_Now)), self->PWM_IDX);
            break;
        case BRAKE:
            // DL_GPIO_clearPins(self->SLP_PORT, self->SLP_PIN);
            DL_GPIO_setPins(self->DIR_PORT, self->DIR_PIN);
            DL_GPIO_setPins(self->SLP_PORT, self->SLP_PIN);
            DL_TimerG_setCaptureCompareValue(self->PWM_INST, 39999, self->PWM_IDX);
            break;
        default:
            break;
        }
    }
   

}


/**
 * @brief 输出PWM
 *
 */
void Motor_Output(Class_Motor *self)
{

    if (self->Output_Now > self->Output_Max)
        self->Output_Now = self->Output_Max;
    if(self->Output_Now < -self->Output_Max)
        self->Output_Now = -self->Output_Max;

    if(fabs(self->Target_Speed) <= 0.0025)
        self->Output_Now = 0;

    switch(self->Side){
        // case RIGHT:
        {
            if (self->Output_Now < -0.1)
            {
                self->Control(self, FORWARD);
            }else if(self->Output_Now > 0.1){
                self->Control(self, BACKWARD);
            }else{
                self->Control(self, BRAKE);
            }
        }
        break;
        // case LEFT:
        {
            if (self->Output_Now > 0.1){
                self->Control(self, BACKWARD);
            }else if(self->Output_Now < -0.1){
                self->Control(self, FORWARD);
            }else{
                self->Control(self, BRAKE);
            }
        }
        break;
        default:
            break;
    }
}

/**
 * @brief 编码器计算回调函数
 *
 * @param phase 编码器相位
 */
void Motor_Encoder_Callback(Class_Motor *self, char phase)
{
    switch (phase)
    {
    case 'A':
    {
        if (DL_GPIO_readPins(self->ENCODER_A_PORT, self->ENCODER_A_PIN))
        {
            if (DL_GPIO_readPins(self->ENCODER_B_PORT, self->ENCODER_B_PIN))
            {
                self->Total_Encoder_Tick++;
            }
            else
            {
                self->Total_Encoder_Tick--;
            }
        }
        else
        {
            if (DL_GPIO_readPins(self->ENCODER_B_PORT, self->ENCODER_B_PIN))
            {
                self->Total_Encoder_Tick--;
            }
            else
            {
                self->Total_Encoder_Tick++;
            }
        }
    }
    break;
    case 'B':
    {
        if (DL_GPIO_readPins(self->ENCODER_B_PORT, self->ENCODER_B_PIN))
        {
            if (DL_GPIO_readPins(self->ENCODER_A_PORT, self->ENCODER_A_PIN))
            {
                self->Total_Encoder_Tick--;
            }
            else
            {
                self->Total_Encoder_Tick++;
            }
        }
        else
        {
            if (DL_GPIO_readPins(self->ENCODER_A_PORT, self->ENCODER_A_PIN))
            {
                self->Total_Encoder_Tick++;
            }
            else
            {
                self->Total_Encoder_Tick--;
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
void Motor_TIM_PID_PeriodElapsedCallback(Class_Motor *self)
{
    if(self->Target_Speed > self->Speed_Max)
    {
        self->Target_Speed = self->Speed_Max;
    }
    if (self->Target_Speed < -self->Speed_Max)
    {
        self->Target_Speed = -self->Speed_Max;
    }

    self->PID_Speed->Set_Target((self->PID_Speed), self->Target_Speed);
    self->PID_Speed->Set_Now((self->PID_Speed), self->Now_Speed);
    self->PID_Speed->TIM_Adjust_PeriodElapsedCallback((self->PID_Speed));

    self->Output_Now = (self->PID_Speed->Get_PID_Out((self->PID_Speed)));

    self->Output(self);
}
