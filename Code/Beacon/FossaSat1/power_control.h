#ifndef POWER_CONTROL_H_INCLUDED
#define POWER_CONTROL_H_INCLUDED

#include "FossaSat1.h"

struct powerConfigBits_t {
  uint8_t lowPowerModeActive : 1;       // LSB
  uint8_t lowPowerModeEnabled : 1;
  uint8_t mpptTempSwitchEnabled : 1;
  uint8_t mpptKeepAliveEnabled : 1;
  uint8_t transmitEnabled : 1;
};

union powerConfig_t {
  struct powerConfigBits_t bits;
  uint8_t val;
};

extern powerConfig_t powerConfig;

void Power_Control_Load_Configuration();
void Power_Control_Save_Configuration();

void Power_Control_Charge(bool charge);
uint32_t Power_Control_Get_Sleep_Interval();

void Power_Control_Delay(uint32_t ms, bool sleep);

void Power_Control_Setup_INA226();
bool Power_Control_INA2256_Check();
float Power_Control_Get_Battery_Voltage();
float Power_Control_Get_Charging_Voltage();
float Power_Control_Get_Charging_Current();

#endif
