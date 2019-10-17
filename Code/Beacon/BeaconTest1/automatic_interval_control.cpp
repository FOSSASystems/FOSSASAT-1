#include "automatic_interval_control.h"
#include "debugging_utilities.h"
#include "power_control.h"
#include "configuration.h"
#include "pin_interface.h"

int Automatic_Interval_Control_GetInterval()
{
  float batteryVoltageReading = Pin_Interface_GetBatteryVoltage();

  DEBUG_PRINTLN(F("Battery voltage reading:"));
  DEBUG_PRINTLN(batteryVoltageReading);

  int intervalValue = 0; // normal reading (> 3.7V) 0 interval delay

#ifdef ENABLE_INTERVAL_CONTROL
  if (batteryVoltageReading <= 3.3f)
  {
    intervalValue = 40 * 1000;
  }
  if (batteryVoltageReading < 3.4f)
  {
    intervalValue = 30 * 1000; // 30 second wait
  }
  else if (batteryVoltageReading < 3.5f)
  {
    intervalValue = 20 * 1000; // 20 seconds wait.
  }
  else if (batteryVoltageReading < 3.6f)
  {
    intervalValue = 10 * 1000; // 10 second wait.
  }
  else if (batteryVoltageReading < 3.7f)
  {
    intervalValue = 5 * 1000; // 5 seconds wait
  }
#endif

  return intervalValue;
}
