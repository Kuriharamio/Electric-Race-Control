#include "Base_Modules/adc_button.h"

#ifdef USE_ADC_BUTTON
// <<<<<<<<<   实例声明     <<<<<<<<<<<
Class_ADCButton _ADC_Button = {0};
// >>>>>>>>>   实例声明     >>>>>>>>>>>

// static Button Button_map[5] = {
//     {2600, 2900, BUTTON_1, false, BUTTON_IDLE, 0}, // 2719 upup
//     {1950, 2250, BUTTON_2, false, BUTTON_IDLE, 0}, // 2160 up
//     {1500, 1850, BUTTON_3, false, BUTTON_IDLE, 0}, // 1626 left
//     {400, 700, BUTTON_4, false, BUTTON_IDLE, 0}, // 550 right
//     {950, 1250, BUTTON_5, false, BUTTON_IDLE, 0}   // 1080 down
// };

static Button Button_map[5] = {
    {2500, 2900, BUTTON_1, false, BUTTON_IDLE, 0}, //  upup AAA 2730
    {1900, 2300, BUTTON_2, false, BUTTON_IDLE, 0}, //  up 866 2150
    {1400, 1800, BUTTON_3, false, BUTTON_IDLE, 0}, //  left 649  1609
    {400, 700, BUTTON_4, false, BUTTON_IDLE, 0},   //  right 217 535
    {800, 1200, BUTTON_5, false, BUTTON_IDLE, 0}   //  down 414  1044
};

/**
 *@brief 创建adc按键实例
 */
pClass_ADCButton Create_ADCButton(void)
{
    pClass_ADCButton ADC_Button = &_ADC_Button;

    ADC_Button->Init = ADC_Init;
    ADC_Button->Get_Current_Value = ADC_Get_Current_Value;
    ADC_Button->Get_Current_Button = ADC_Get_Current_Button;

    ADC_Button->Configure_Callback = ADC_Configure_Callback;
    ADC_Button->Check_And_Trigger = ADC_Check_And_Trigger;

    return ADC_Button;
}

pClass_ADCButton GET_ADCButton_INST(void)
{
    return &_ADC_Button;
}

/**
 *@brief 初始化adc按键实例
 *
 *@param this
 */
void ADC_Init(pClass_ADCButton this, ADC12_Regs *ADC_INST, IRQn_Type ADC_INST_INT_IRQN, DL_ADC12_MEM_IDX ADCMEM_IDX)
{
    this->ADC_Button_INST = ADC_INST;
    this->ADC_Button_IRQN = ADC_INST_INT_IRQN;
    this->ADCMEM_IDX = ADCMEM_IDX;

    NVIC_ClearPendingIRQ(this->ADC_Button_IRQN);
    NVIC_EnableIRQ(this->ADC_Button_IRQN);

    this->ADC_Flag = false;
    this->Current_ADC_Value = 0;

    for (int i = 0; i < 5; ++i)
    {
        this->Short_Callbacks[i] = NULL;
        this->Long_Callbacks[i] = NULL;
        this->Long_Continuous_Callback[i] = NULL;
    }

    this->is_inited = true;
}

/**
 *@brief 获取adc按键的值
 *
 *@param this
 */
void ADC_Get_Current_Value(pClass_ADCButton this)
{
    DL_ADC12_enableConversions(this->ADC_Button_INST);
    DL_ADC12_startConversion(this->ADC_Button_INST);

    // 如果 ADC 转换未完成，直接返回
    if (this->ADC_Flag == false)
    {
        return;
    }

    this->Current_ADC_Value = DL_ADC12_getMemResult(this->ADC_Button_INST, this->ADCMEM_IDX);
    this->ADC_Flag = false;
}

/**
 *@brief 判断相应模式
 *
 *@param this
 */
void ADC_Get_Current_Button(pClass_ADCButton this)
{
    this->Get_Current_Value(this);

    for (size_t i = 0; i < sizeof(Button_map) / sizeof(Button_map[0]); ++i)
    {
        if (this->Current_ADC_Value >= Button_map[i].min_val && this->Current_ADC_Value <= Button_map[i].max_val)
        {
            Button_map[i].Is_Being_Pressed = true;
        }
        else
        {
            Button_map[i].Is_Being_Pressed = false;
        }
    }
}

/**
 *@brief 设置按键回调函数(设置回调函数里的参数是为了测试)
 *@param this
 *@param mode
 *@param (*callback)
 *
 */
void ADC_Configure_Callback(pClass_ADCButton this, ButtonNum Button, void (*short_callback)(void), void (*long_callback)(void), void (*long_continuous_callback)(void))
{
    if (Button >= BUTTON_1 && Button <= BUTTON_5)
    {
        this->Short_Callbacks[Button] = short_callback;
        this->Long_Callbacks[Button] = long_callback;
        this->Long_Continuous_Callback[Button] = long_continuous_callback;
    }
}

/**
 *@brief 检查当前按键并触发回调
 *
 *@param this
 */
void ADC_Check_And_Trigger(pClass_ADCButton this)
{
    this->Get_Current_Button(this);

    for(int i = 0; i < 5; ++i){
        switch (Button_map[i].State)
        {
        case BUTTON_IDLE:
        {
            if(Button_map[i].Is_Being_Pressed){
                Button_map[i].State = BUTTON_DEBOUNCE;
                Button_map[i].Timer = 0;
            }         
        }
        break;
        case BUTTON_DEBOUNCE:
        {
            if (Button_map[i].Is_Being_Pressed)
            {
                Button_map[i].Timer++;
                if (Button_map[i].Timer >= 5)
                {
                    Button_map[i].State = BUTTON_PRESSED;
                }
            }
            else
            {
                Button_map[i].State = BUTTON_IDLE;
            }
        }
        break;
        case BUTTON_PRESSED:
        {
            if (Button_map[i].Is_Being_Pressed)
            {
                Button_map[i].Timer++;
                if (Button_map[i].Timer >= LONG_PRESS_TIME / BUTTON_DELTA_T)
                {
                    Button_map[i].State = BUTTON_LONG_PRESSED;
                }
            }
            else
            {
                if (this->Short_Callbacks[i] != NULL)
                {
                    this->Short_Callbacks[i]();
                }
                Button_map[i].State = BUTTON_IDLE;
            }
        }
        break;
        case BUTTON_LONG_PRESSED:
        {
            if(this->Long_Continuous_Callback[i] != NULL){
                this->Long_Continuous_Callback[i]();
            }
            if (!Button_map[i].Is_Being_Pressed)
            {
                if (this->Long_Callbacks[i] != NULL)
                {
                    this->Long_Callbacks[i]();
                }
                Button_map[i].State = BUTTON_IDLE;
            }
        }
        break;
        default:
            break;
        }
    }
}

/**
 *@brief adc中断处理函数
 *
 *
 */
void ADC_BUTTON_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_BUTTON_INST))
    {
    case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
        DL_ADC12_clearInterruptStatus(ADC_BUTTON_INST, DL_ADC12_IIDX_MEM0_RESULT_LOADED);
        if (_ADC_Button.is_inited)
        {
            _ADC_Button.ADC_Flag = true;
        }
        break;
    default:
        break;
    }
}

// /*********************
//  *   自定义按键回调  *
//  *********************/
// #include "Base_Modules/servo.h"
//  //* 短按
//  void Button_1_Short_Callback(void)
//  {
//  }

//  void Button_2_Short_Callback(void)
//  {
//  }

//  void Button_3_Short_Callback(void)
//  {
//  }

//  void Button_4_Short_Callback(void)
//  {
//  }

//  void Button_5_Short_Callback(void)
//  {
//  }


//  //* 长按
//  void Button_1_Long_Callback(void)
//  {
//  }

//  void Button_2_Long_Callback(void)
//  {
//  }

//  void Button_3_Long_Callback(void)
//  {
//  }

//  void Button_4_Long_Callback(void)
//  {
//  }

//  void Button_5_Long_Callback(void)
//  {
//  }

//  //* 长按连续
// void Button_1_Long_Continuous_Callback(void)
// {
// }

// void Button_2_Long_Continuous_Callback(void)
// {
//     pClass_Servo servo = create_Servo(SERVO_UP_INDEX);
//     if (servo->is_inited)
//         servo->Set_Angle(servo, servo->Now_Angle - 0.1);
// }

// void Button_3_Long_Continuous_Callback(void)
// {
//     pClass_Servo servo = create_Servo(SERVO_DOWN_INDEX);
//     if (servo->is_inited)
//         servo->Set_Angle(servo, servo->Now_Angle + 0.1);
// }

// void Button_4_Long_Continuous_Callback(void)
// {
//     pClass_Servo servo = create_Servo(SERVO_DOWN_INDEX);
//     if (servo->is_inited)
//         servo->Set_Angle(servo, servo->Now_Angle - 0.1);
// }

// void Button_5_Long_Continuous_Callback(void)
// {
//     pClass_Servo servo = create_Servo(SERVO_UP_INDEX);
//     if (servo->is_inited)
//         servo->Set_Angle(servo, servo->Now_Angle + 0.1);
// }

#endif