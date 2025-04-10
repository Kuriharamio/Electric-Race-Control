#ifndef __ADC_BUTTON_H
#define __ADC_BUTTON_H

#include "ti_msp_dl_config.h"
#include "BSP/delay.h"
#include <stdlib.h>

typedef enum
{
	BUTTON_None,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
} ButtonNum;

typedef struct
{
	uint16_t min_val;
	uint16_t max_val;
	ButtonNum Button;
} AdcButtonThreshold;

typedef struct Class_ADCButton
{
	ADC12_Regs *ADC_Button_INST;
	IRQn_Type ADC_Button_IRQN;
	DL_ADC12_MEM_IDX ADCMEM_IDX;
	bool ADC_Flag;
	unsigned int Current_ADC_Value;
	ButtonNum Current_Button;
	bool is_inited;

	void (*Init)(struct Class_ADCButton *this, ADC12_Regs *ADC_INST, IRQn_Type ADC_INST_INT_IRQN, DL_ADC12_MEM_IDX ADCMEM_IDX);
	void (*Get_Current_Value)(struct Class_ADCButton *this);
	void (*Get_Current_Button)(struct Class_ADCButton *this);
	void (*Configure_Callback)(struct Class_ADCButton *this, ButtonNum Button, void (*callback)(void));
	void (*Check_And_Trigger)(struct Class_ADCButton *this);

	void (*Callbacks[5])(void);

} Class_ADCButton, *pClass_ADCButton;

pClass_ADCButton Create_AdcButton(void);
pClass_ADCButton GET_ADCButton_INST(void);

void Adc_Init(pClass_ADCButton this, ADC12_Regs *ADC_INST, IRQn_Type ADC_INST_INT_IRQN, DL_ADC12_MEM_IDX ADCMEM_IDX);
void Adc_Get_Current_Value(pClass_ADCButton this);
void Adc_Get_Current_Button(pClass_ADCButton this);
void Adc_Configure_Callback(pClass_ADCButton this, ButtonNum Button, void (*callback)(void));
void Adc_Check_And_Trigger(pClass_ADCButton this);

#endif // __ADC_BUTTON_H