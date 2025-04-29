#ifndef __ADC_BUTTON_H
#define __ADC_BUTTON_H

#include "config.h"

typedef enum
{
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_None
} ButtonNum;

typedef enum
{
	BUTTON_IDLE,
	BUTTON_DEBOUNCE,
	BUTTON_PRESSED,
	BUTTON_LONG_PRESSED
} ButtonState;

typedef struct
{
	const uint16_t min_val;
	const uint16_t max_val;
	const ButtonNum Num;
	bool Is_Being_Pressed;
	ButtonState State;
	uint8_t Timer;
} Button;

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
	void (*Configure_Callback)(struct Class_ADCButton *this, ButtonNum Button, void (*short_callback)(void), void (*long_callback)(void), void (*long_continuous_callback)(void));
	void (*Check_And_Trigger)(struct Class_ADCButton *this);

	void (*Short_Callbacks[5])(void);
	void (*Long_Callbacks[5])(void);
	void (*Long_Continuous_Callback[5])(void);

} Class_ADCButton, *pClass_ADCButton;

pClass_ADCButton Create_ADCButton(void);
pClass_ADCButton GET_ADCButton_INST(void);

void ADC_Init(pClass_ADCButton this, ADC12_Regs *ADC_INST, IRQn_Type ADC_INST_INT_IRQN, DL_ADC12_MEM_IDX ADCMEM_IDX);
void ADC_Get_Current_Value(pClass_ADCButton this);
void ADC_Get_Current_Button(pClass_ADCButton this);
void ADC_Configure_Callback(pClass_ADCButton this, ButtonNum Button, void (*short_callback)(void), void (*long_callback)(void), void (*long_continuous_callback)(void));
void ADC_Check_And_Trigger(pClass_ADCButton this);

// void Button_1_Short_Callback(void);
// void Button_1_Long_Callback(void);
// void Button_1_Long_Continuous_Callback(void);

// void Button_2_Short_Callback(void);
// void Button_2_Long_Callback(void);
// void Button_2_Long_Continuous_Callback(void);

// void Button_3_Short_Callback(void);
// void Button_3_Long_Callback(void);
// void Button_3_Long_Continuous_Callback(void);

// void Button_4_Short_Callback(void);
// void Button_4_Long_Callback(void);
// void Button_4_Long_Continuous_Callback(void);

// void Button_5_Short_Callback(void);
// void Button_5_Long_Callback(void);
// void Button_5_Long_Continuous_Callback(void);

#endif // __ADC_BUTTON_H