#ifndef PIN_INTERFACE_H_INCLUDED
#define PIN_INTERFACE_H_INCLUDED

extern bool WATCHDOG_HEARTBEAT_STATE;

int Pin_Interface_GetBatteryChargingVoltage();
int Pin_Interface_GetBatteryVoltage();
int Pin_Interface_GetTotalSolarCellVoltage();
bool Pin_Interface_ShouldReset();

void Pin_Interface_WatchdogHeartbeat();
#endif
