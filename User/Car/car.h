#ifndef __CAR_H__
#define __CAR_H__

#include "Base_Modules/motor.h"

typedef struct Class_Car
{
   

  
    void (*Init)(struct Class_Car *self);
    void (*TIM_PID_PeriodElapsedCallback)(struct Class_Car *self);

} Class_Car;

Class_Car *create_car(void);
void Car_Init(Class_Car *self);
void Car_TIM_PID_PeriodElapsedCallback(Class_Car *self);

#endif