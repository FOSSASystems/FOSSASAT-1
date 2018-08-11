#ifndef PIN_INTERFACE_H_INCLUDED
#define PIN_INTERFACE_H_INCLUDED

extern bool WATCHDOG_HEARTBEAT_STATE;

String Pin_Interface_GetBatteryChargingVoltage();
String Pin_Interface_GetBatteryVoltage();
String Pin_Interface_GetTotalSolarCellVoltage();
bool Pin_Interface_ShouldReset();

void Pin_Interface_WatchdogHeartbeat();
#endif
