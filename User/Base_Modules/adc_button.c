#include "Base_Modules/adc_button.h"
#include "BSP/delay.h"
#include <stdlib.h>

static pClass_AdcKey g_AdcKeyInstance = NULL;

static const AdcKeyThreshold key_map[] = {
    {4000, 5000, Mode_1},
    {3000, 3500, Mode_2},
    {2200, 2600, Mode_3},
    {1200, 1700, Mode_4},
    {500,  1000, Mode_5}
};

/**
 *@brief 创建adc按键实例
 */
pClass_AdcKey Create_AdcKey(void)
{
	pClass_AdcKey AdcKey = (pClass_AdcKey)malloc(sizeof(Class_AdcKey));
	
	if(AdcKey==NULL)
	{
		return NULL;
	}
	
	AdcKey->Init = Adc_Init;
	AdcKey->Get_Value = Adc_Get_Value;
	AdcKey->Get_Mode = Adc_Get_Mode;
	
	AdcKey->Set_Callback = Adc_Set_Callback;
    AdcKey->Check_And_Trigger = Adc_Check_And_Trigger;

    for (int i = 0; i < 5; ++i) {
        AdcKey->Callbacks[i] = NULL;
    }

	
	return AdcKey;
}

/**
 *@brief 初始化adc按键实例
 *
 *@param this
 */
void Adc_Init(pClass_AdcKey this)
{
	NVIC_EnableIRQ(adckey_INST_INT_IRQN);
	
	this->AdcKey_INST = adckey_INST;
	this->AdcFlag = false;
	this->Mode = Mode_None;
	this->Adc_Value = 0;
	
	g_AdcKeyInstance = this;
	
}

/**
 *@brief 获取adc按键的值
 *
 *@param this
 */
void Adc_Get_Value(pClass_AdcKey this)
{
	this->Adc_Value=0;
	
	DL_ADC12_enableConversions(this->AdcKey_INST);
	DL_ADC12_startConversion(this->AdcKey_INST);
	
	while(this->AdcFlag==false)
	{
		__WFE();
	}
	
	this->Adc_Value=DL_ADC12_getMemResult(adckey_INST,adckey_ADCMEM_key);
	
	this->AdcFlag=false;
}

/**
 *@brief 判断相应模式
 *
 *@param this
 */
void Adc_Get_Mode(pClass_AdcKey this)
{
    this->Get_Value(this);

    for (size_t i = 0; i < sizeof(key_map) / sizeof(key_map[0]); ++i) {
        if (this->Adc_Value >= key_map[i].min_val && this->Adc_Value <= key_map[i].max_val) {
            delay_ms(10);
            this->Get_Value(this);  // 重新采样确认
            if (this->Adc_Value >= key_map[i].min_val && this->Adc_Value <= key_map[i].max_val) {
                this->Mode = key_map[i].mode;
                return;
            }
        }
    }

    //this->Mode = Mode_None;
}

/**
 *@brief 设置按键回调函数
 *@param this 
 *@param mode
 *@param (*callback)
 *
 */
void Adc_Set_Callback(pClass_AdcKey this, KeyMode mode, void (*callback)(void))
{
    if (mode >= Mode_1 && mode <= Mode_5) {
        this->Callbacks[mode - 1] = callback;
    }
}

/**
 *@brief 检查当前按键并触发回调
 *
 *@param this
 */
void Adc_Check_And_Trigger(pClass_AdcKey this)
{
    this->Get_Mode(this);

    if (this->Mode >= Mode_1 && this->Mode <= Mode_5) {
        void (*cb)(void) = this->Callbacks[this->Mode - 1];
        if (cb) {
            cb();
        }
    }
}
/**
 *@brief adc中断处理函数
 *
 *
 */
void adckey_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(adckey_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            DL_ADC12_clearInterruptStatus(adckey_INST, DL_ADC12_IIDX_MEM0_RESULT_LOADED);
            if (g_AdcKeyInstance) {
                g_AdcKeyInstance->AdcFlag = true;
            }
            break;
        default:
            break;
    }
}
			