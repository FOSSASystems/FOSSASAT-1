#ifndef PIN_INTERFACE_H_INCLUDED
#define PIN_INTERFACE_H_INCLUDED

#include "FossaSat1.h"

void Pin_Interface_Set_Temp_Resolution(uint8_t sensorAddr, uint8_t res);
float Pin_Interface_Read_Temperature(uint8_t sensorAddr);
int8_t Pin_Interface_Read_Temperature_Internal();

float Pin_Interface_Read_Voltage(uint8_t pin);

void Pin_Interface_Watchdog_Heartbeat();
void Pin_Interface_Watchdog_Restart();

#endif
