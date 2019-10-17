#ifndef PIN_INTERFACE_H_INCLUDED
#define PIN_INTERFACE_H_INCLUDED

#include <Arduino.h>

extern bool WATCHDOG_HEARTBEAT_STATE;

float Pin_Interface_GetBatteryChargingVoltage();
float Pin_Interface_GetBatteryVoltage();
float Pin_Interface_GetSolarCellAVoltage();
float Pin_Interface_GetSolarCellBVoltage();
float Pin_Interface_GetSolarCellCVoltage();

bool Pin_Interface_ShouldReset();

void Pin_Interface_WatchdogHeartbeat();

void Pin_Interface_Set_Board_Temperature_Res();
void Pin_Interface_Set_Battery_Temperature_Res();

float Pin_Interface_GetBatteryTemperature();
float Pin_Interface_GetBoardTemperature();

#endif
