#include "i2c.h"
void hardware_IIC_WirteByte(uint8_t addr, uint8_t dat)
{	
    uint8_t temp[2];
    temp[0] = addr;
    temp[1] = dat;
    DL_I2C_fillControllerTXFIFO(I2C0, temp, 2);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C0, Sensor_Adrees, DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    while (DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C0);
}
void hardware_IIC_WirteBytes(uint8_t addr, uint8_t * dat, uint8_t len)
{
		uint8_t temp[len+1];
	  temp[0] = addr;
		memcpy(temp+1,dat,len);
    DL_I2C_fillControllerTXFIFO(I2C0, temp, len+1);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C0, Sensor_Adrees, DL_I2C_CONTROLLER_DIRECTION_TX, len+1);
    while (DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C0);
}
uint8_t hardware_IIC_ReadByte(uint8_t addr) 
{
    uint8_t data;
    DL_I2C_fillControllerTXFIFO(I2C0, &addr, 1);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C0, Sensor_Adrees, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    while (DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C0);
 
    DL_I2C_startControllerTransfer(I2C0, Sensor_Adrees, DL_I2C_CONTROLLER_DIRECTION_RX, 1);
    while (DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    data = DL_I2C_receiveControllerData(I2C0);
 
    return data;
}
void hardware_IIC_ReadBytes(uint8_t addr,uint8_t *dat,uint8_t len) 
{ 
    DL_I2C_fillControllerTXFIFO(I2C0, &addr, 1);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C0, Sensor_Adrees, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    while (DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C0);
 
    DL_I2C_startControllerTransfer(I2C0, Sensor_Adrees, DL_I2C_CONTROLLER_DIRECTION_RX, len);
    while (DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C0) & DL_I2C_CONTROLLER_STATUS_IDLE));
	
		for (uint8_t i = 0; i < len; i++) {
    dat[i] = DL_I2C_receiveTargetData(I2C0);
		}
}