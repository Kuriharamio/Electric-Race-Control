#ifndef __ADC_BUTTON_H
#define __ADC_BUTTON_H


#include "ti_msp_dl_config.h"
#include "Base_Modules/bluetooth.h"

typedef enum{
	Mode_None = 0,
	Mode_1,
	Mode_2,
	Mode_3,
	Mode_4,
	Mode_5,
}KeyMode;

typedef struct {
    uint16_t min_val;
    uint16_t max_val;
    KeyMode mode;
} AdcKeyThreshold;

typedef struct Class_AdcKey{
	
	ADC12_Regs * AdcKey_INST;
	bool AdcFlag;
	KeyMode Mode;
	unsigned int Adc_Value;
	pClass_Bluetooth Bluetooth;
	
	void (*Init)(struct Class_AdcKey* this,pClass_Bluetooth that);
	void (*Get_Value)(struct Class_AdcKey* this);
	void (*Get_Mode)(struct Class_AdcKey* this);
	void (*Set_Callback)(struct Class_AdcKey *this, KeyMode mode, void (*callback)(pClass_Bluetooth this));
    void (*Check_And_Trigger)(struct Class_AdcKey *this);
	
	void (*Callbacks[5])(pClass_Bluetooth this);

}Class_AdcKey,*pClass_AdcKey;

pClass_AdcKey Create_AdcKey(void);
void Adc_Init(pClass_AdcKey this,pClass_Bluetooth that);
void Adc_Get_Value(pClass_AdcKey this);
void Adc_Get_Mode(pClass_AdcKey this);
void Adc_Set_Callback(pClass_AdcKey this, KeyMode mode, void (*callback)(pClass_Bluetooth this));
void Adc_Check_And_Trigger(pClass_AdcKey this);
#endif