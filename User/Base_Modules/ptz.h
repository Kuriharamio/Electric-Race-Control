#ifndef PTZ_H
#define PTZ_H 

#include "config.h"
#include "FT_servo.h"

typedef struct Class_PTZ {
    
    pClass_FT_Servo Servo_Down;
    pClass_FT_Servo Servo_Up;

    pClass_UART FT_Servo_Controller;

    uint8_t ID[2];
    int16_t Position[2];
    uint16_t Speed[2];
    uint8_t ACC[2];

    bool is_inited;

    void (*Init)(struct Class_PTZ* this);
    void (*Update)(struct Class_PTZ *this);

} Class_PTZ, *pClass_PTZ;

pClass_PTZ Create_PTZ(void);
pClass_PTZ Get_PTZ_INST(void);
void PTZ_Init(pClass_PTZ this);
void PTZ_Update(pClass_PTZ this);

#endif