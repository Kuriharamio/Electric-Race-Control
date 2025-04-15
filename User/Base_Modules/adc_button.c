#include "Base_Modules/adc_button.h"

// <<<<<<<<<   实例声明     <<<<<<<<<<<
static Class_ADCButton _ADC_Button = {0};
// >>>>>>>>>   实例声明     >>>>>>>>>>>

static const AdcButtonThreshold Button_map[5] = {
    {4000, 5000, BUTTON_1},
    {3000, 3500, BUTTON_2},
    {2200, 2600, BUTTON_3},
    {1200, 1700, BUTTON_4},
    {500, 1000, BUTTON_5}};

/**
 *@brief 创建adc按键实例
 */
pClass_ADCButton Create_AdcButton(void)
{
    pClass_ADCButton AdcButton = &_ADC_Button;

    AdcButton->Init = Adc_Init;
    AdcButton->Get_Current_Value = Adc_Get_Current_Value;
    AdcButton->Get_Current_Button = Adc_Get_Current_Button;

    AdcButton->Configure_Callback = Adc_Configure_Callback;
    AdcButton->Check_And_Trigger = Adc_Check_And_Trigger;

    return AdcButton;
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
void Adc_Init(pClass_ADCButton this, ADC12_Regs *ADC_INST, IRQn_Type ADC_INST_INT_IRQN, DL_ADC12_MEM_IDX ADCMEM_IDX)
{
    this->ADC_Button_INST = ADC_INST;
    this->ADC_Button_IRQN = ADC_INST_INT_IRQN;
    this->ADCMEM_IDX = ADCMEM_IDX;

    NVIC_EnableIRQ(this->ADC_Button_IRQN);

    this->ADC_Flag = false;
    this->Current_Button = BUTTON_None;
    this->Current_ADC_Value = 0;

    for (int i = 0; i < 5; ++i)
    {
        this->Callbacks[i] = NULL;
    }

    this->is_inited = true;
}

/**
 *@brief 获取adc按键的值
 *
 *@param this
 */
void Adc_Get_Current_Value(pClass_ADCButton this)
{
    static uint32_t adc_sum = 0;
    static uint8_t sample_count = 0;

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
void Adc_Get_Current_Button(pClass_ADCButton this)
{
    this->Get_Current_Value(this);

    for (size_t i = 0; i < sizeof(Button_map) / sizeof(Button_map[0]); ++i)
    {
        if (this->Current_ADC_Value >= Button_map[i].min_val && this->Current_ADC_Value <= Button_map[i].max_val)
        {
            this->Current_Button = Button_map[i].Button;
            return;
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
void Adc_Configure_Callback(pClass_ADCButton this, ButtonNum Button, void (*callback)(void))
{
    if (Button >= BUTTON_1 && Button <= BUTTON_5)
    {
        this->Callbacks[Button - 1] = callback;
    }
}

/**
 *@brief 检查当前按键并触发回调
 *
 *@param this
 */
void Adc_Check_And_Trigger(pClass_ADCButton this)
{
    this->Get_Current_Button(this);

    if (this->Current_Button >= BUTTON_1 && this->Current_Button <= BUTTON_5)
    {
        this->Callbacks[this->Current_Button - 1]();
        this->Current_ADC_Value = 0;
        this->Current_Button = BUTTON_None;
    }
}

/**
 *@brief adc中断处理函数
 *
 *
 */
void adckey_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(adckey_INST))
    {
    case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
        DL_ADC12_clearInterruptStatus(adckey_INST, DL_ADC12_IIDX_MEM0_RESULT_LOADED);
        if (_ADC_Button.is_inited)
        {
            _ADC_Button.ADC_Flag = true;
        }
        break;
    default:
        break;
    }
}
