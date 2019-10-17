#ifndef POWER_CONTROL_H_INCLUDED
#define POWER_CONTROL_H_INCLUDED


extern bool BATTERY_CHARGING_SWITCHED_OFF;
extern bool LOW_POWER_MODE;
extern bool LOW_POWER_MODE_ENABLED;
extern bool MPPT_TEMPERATURE_SWITCH_ENABLED;
extern bool MPPT_KEEP_ALIVE_MODE_ENABLED;

void Power_Control_LoadConfiguration();
void Power_Control_SaveConfiguration();

void Power_Control_Delay(unsigned long pMillisDelay);
void Power_Control_Sleep(unsigned long pMillisDelay);
void Power_Control_CheckBatteryTemperature();

void Power_Control_SwitchOnMPPT();
void Power_Control_SwitchOffMPPT();

void Power_Control_EnableMPPTTemperatureSwitch();
void Power_Control_DisableMPPTTemperatureSwitch();
bool Power_Control_IsMPPTTemperatureSwitchEnabled();

void Power_Control_TurnOnMPPTKeepAlive();
void Power_Control_TurnOffMPPTKeepAlive();
void Power_Control_PrintConfiguration();

// Setters
void Power_Control_TurnOnLowPowerMode();
void Power_Control_TurnOffLowPowerMode();

void Power_Control_EnableLowPowerMode();
void Power_Control_DisableLowPowerMode();

// Getters
bool Power_Control_IsLowPowerModeOn();
bool Power_Control_IsLowPowerModeEnabled();
bool Power_Control_IsLowPowerModeOperational();
#endif
