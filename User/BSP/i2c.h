#include "config.h"

#define Sensor_Adrees 0x4c
void hardware_IIC_WirteByte(uint8_t addr, uint8_t dat);
void hardware_IIC_WirteBytes(uint8_t addr, uint8_t * dat, uint8_t len);
void hardware_IIC_ReadBytes(uint8_t addr,uint8_t *dat,uint8_t len) ;