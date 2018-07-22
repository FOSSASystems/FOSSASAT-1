#ifndef PIN_INTERFACE_H_INCLUDED
#define PIN_INTERFACE_H_INCLUDED

extern bool WATCHDOG_HEARTBEAT_STATE;

int Pin_Interface_GetSolarCellVoltage(int inCellIndex);
int Pin_Interface_GetBatteryChargingCurrent();
bool Pin_Interface_ShouldReset();

void Pin_Interface_WatchdogHeartbeat();
#endif
