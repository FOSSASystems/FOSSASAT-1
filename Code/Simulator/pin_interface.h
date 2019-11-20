#ifndef PIN_INTERFACE_H_INCLUDED
#define PIN_INTERFACE_H_INCLUDED

#include "Simulator.h"

int8_t Pin_Interface_Read_Temperature_Internal();

float Pin_Interface_Read_Voltage(uint8_t pin);

#endif
