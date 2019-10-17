#include <LowPower.h>

#include "power_control.h"
#include "configuration.h"
#include "pin_interface.h"
#include "persistant_storage.h"
#include "debugging_utilities.h"

bool BATTERY_CHARGING_SWITCHED_OFF = false;
bool LOW_POWER_MODE = false;
bool LOW_POWER_MODE_ENABLED = true;
bool MPPT_TEMPERATURE_SWITCH_ENABLED = true;
bool MPPT_KEEP_ALIVE_MODE_ENABLED = false;

void Power_Control_LoadConfiguration()
{
  FOSSASAT_DEBUG_PRINTLN(F("Loading power configuration..."));
  if (Persistant_Storage_Get(EEPROM_LOW_POWER_MODE_ADDR) == 1)
  {
    LOW_POWER_MODE = true;
  }
  else
  {
    LOW_POWER_MODE = false;
  }

  if (Persistant_Storage_Get(EEPROM_LOW_POWER_MODE_ENABLED_ADDR) == 1)
  {
    LOW_POWER_MODE_ENABLED = true;
  }
  else
  {
    LOW_POWER_MODE_ENABLED = false;
  }

  if (Persistant_Storage_Get(EEPROM_MPPT_TEMPERATURE_SWITCH_ENABLED_ADDR) == 1)
  {
    MPPT_TEMPERATURE_SWITCH_ENABLED = true;
  }
  else
  {
    MPPT_TEMPERATURE_SWITCH_ENABLED = false;
  }

  if (Persistant_Storage_Get(EEPROM_MPPT_KEEP_ALIVE_MODE_ENABLED_ADDR) == 1)
  {
    MPPT_KEEP_ALIVE_MODE_ENABLED = true;
  }
  else
  {
    MPPT_KEEP_ALIVE_MODE_ENABLED = false;
  }
  FOSSASAT_DEBUG_PRINTLN(F("Done!"));
}

void Power_Control_SaveConfiguration()
{
  FOSSASAT_DEBUG_PRINTLN(F("Saving power configuration..."));
  if (LOW_POWER_MODE)
  {
    if (Persistant_Storage_Get(EEPROM_LOW_POWER_MODE_ADDR) == 0)
    {
        Persistant_Storage_Set(EEPROM_LOW_POWER_MODE_ADDR, 1);
    }
    else
    {
      // already set to 1.
    }
  }
  else
  {
    if (Persistant_Storage_Get(EEPROM_LOW_POWER_MODE_ADDR) == 1)
    {
        Persistant_Storage_Set(EEPROM_LOW_POWER_MODE_ADDR, 0);
    }
    else
    {
      // already set to 0.
    }
  }

  if (LOW_POWER_MODE_ENABLED)
  {
    if (Persistant_Storage_Get(EEPROM_LOW_POWER_MODE_ENABLED_ADDR) == 0)
    {
      Persistant_Storage_Set(EEPROM_LOW_POWER_MODE_ENABLED_ADDR, 1);
    }
    else
    {
      // already set to 1
    }
  }
  else
  {
    if (Persistant_Storage_Get(EEPROM_LOW_POWER_MODE_ENABLED_ADDR) == 1)
    {
      Persistant_Storage_Set(EEPROM_LOW_POWER_MODE_ENABLED_ADDR, 0);
    }
    else
    {
      // already set to 1
    }
  }

  if (MPPT_TEMPERATURE_SWITCH_ENABLED)
  {
    if (Persistant_Storage_Get(EEPROM_MPPT_TEMPERATURE_SWITCH_ENABLED_ADDR) == 0)
    {
      Persistant_Storage_Set(EEPROM_MPPT_TEMPERATURE_SWITCH_ENABLED_ADDR, 1);
    }
    else
    {
      // already set to 1.
    }
  }
  else
  {
    if (Persistant_Storage_Get(EEPROM_MPPT_TEMPERATURE_SWITCH_ENABLED_ADDR) == 1)
    {
      Persistant_Storage_Set(EEPROM_MPPT_TEMPERATURE_SWITCH_ENABLED_ADDR, 0);
    }
    else
    {
      // already set to 0.
    }
  }

  if (MPPT_KEEP_ALIVE_MODE_ENABLED)
  {
    if (Persistant_Storage_Get(EEPROM_MPPT_KEEP_ALIVE_MODE_ENABLED_ADDR) == 0)
    {
      Persistant_Storage_Set(EEPROM_MPPT_KEEP_ALIVE_MODE_ENABLED_ADDR, 1);
    }
    else
    {
      // already set to 1.
    }
  }
  else
  {
    if (Persistant_Storage_Get(EEPROM_MPPT_KEEP_ALIVE_MODE_ENABLED_ADDR) == 1)
    {
      Persistant_Storage_Set(EEPROM_MPPT_KEEP_ALIVE_MODE_ENABLED_ADDR, 0);
    }
    else
    {
      // already set to 0.
    }
  }
  FOSSASAT_DEBUG_PRINTLN(F("Done!"));
}

void Power_Control_PrintConfiguration()
{
  FOSSASAT_DEBUG_PRINTLN(LOW_POWER_MODE);
  FOSSASAT_DEBUG_PRINTLN(LOW_POWER_MODE_ENABLED);
  FOSSASAT_DEBUG_PRINTLN(MPPT_TEMPERATURE_SWITCH_ENABLED);
  FOSSASAT_DEBUG_PRINTLN(MPPT_KEEP_ALIVE_MODE_ENABLED);
}

void Power_Control_TurnOnMPPTKeepAlive()
{
  MPPT_KEEP_ALIVE_MODE_ENABLED = true;
  Power_Control_SaveConfiguration();
}

void Power_Control_TurnOffMPPTKeepAlive()
{
  MPPT_KEEP_ALIVE_MODE_ENABLED = false;
  Power_Control_SaveConfiguration();
}

void Power_Control_TurnOnLowPowerMode()
{
  FOSSASAT_DEBUG_PRINTLN(F("turned on LPM"));
  LOW_POWER_MODE = true;
  Power_Control_SaveConfiguration();
}

void Power_Control_TurnOffLowPowerMode()
{
  FOSSASAT_DEBUG_PRINTLN(F("turned off LPM"));
  LOW_POWER_MODE = false;
  Power_Control_SaveConfiguration();
}

void Power_Control_EnableLowPowerMode()
{
  FOSSASAT_DEBUG_PRINTLN(F("enabled LPM"));
  LOW_POWER_MODE_ENABLED = true;
  Power_Control_SaveConfiguration();
}

void Power_Control_DisableLowPowerMode()
{
  FOSSASAT_DEBUG_PRINTLN(F("disabled LPM"));
  LOW_POWER_MODE_ENABLED = false;
  Power_Control_SaveConfiguration();
}

// INTERNAL USE ONLY
bool Power_Control_IsLowPowerModeOn()
{
  return LOW_POWER_MODE;
}

// INTERNAL USE ONLY
bool Power_Control_IsLowPowerModeEnabled()
{
  return LOW_POWER_MODE_ENABLED;
}

bool Power_Control_IsLowPowerModeOperational()
{
  // low power mode condition.
  bool lowPowerModeEnabled = Power_Control_IsLowPowerModeEnabled();
  bool isInLowPowerMode = Power_Control_IsLowPowerModeOn();

  if (!lowPowerModeEnabled)
  {
    // normal operation if the low power mode is disabled
    FOSSASAT_DEBUG_PRINTLN("Low power mode disabled");
    return false;
  }
  else
  {
    // conditional if the low power mode is enabled.

    if (isInLowPowerMode) // is it turned on?
    {
      // low power mode enabled and low power mode is turned on
      // restricted operation
      FOSSASAT_DEBUG_PRINTLN("Low power mode on");
      return true; // low power mode operational
    }
    else
    {
      // low power mode is enabled and low power mode is turned off
      // normal operation

      FOSSASAT_DEBUG_PRINTLN("Low power mode off");
      return false; // low power mode turned off.
    }
  }

  return false;
}





void Power_Control_Delay(unsigned long pRequiredMillisDelay)
{
  if (pRequiredMillisDelay == 0) return;

  unsigned long numberOfMillisecondsWaited = 0;

  float numberOfLoopsInDelay = pRequiredMillisDelay / 50.0f;

  // round the number of loops up.

  numberOfLoopsInDelay += 0.5f;

  int numberOfLoopsToPerform = (int)numberOfLoopsInDelay;

  for (int i = 0; i < numberOfLoopsToPerform; i++)
  {
    Pin_Interface_WatchdogHeartbeat();

    delay(50);
  }
}

void Power_Control_Sleep(unsigned long pRequiredMillisDelay)
{
  if (pRequiredMillisDelay == 0) return;

  unsigned long numberOfMillisecondsWaited = 0;

  float numberOfLoopsInDelay = pRequiredMillisDelay / 1000.0f;

  // round the number of loops up.

  numberOfLoopsInDelay += 0.5f;

  int numberOfLoopsToPerform = (int)numberOfLoopsInDelay;

  for (int i = 0; i < numberOfLoopsToPerform; i++)
  {
    Pin_Interface_WatchdogHeartbeat();

    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
}







void Power_Control_EnableMPPTTemperatureSwitch()
{
  FOSSASAT_DEBUG_PRINTLN(F("MPPT temperature switching enabled."));
  MPPT_TEMPERATURE_SWITCH_ENABLED = true;
  Power_Control_SaveConfiguration();
}

void Power_Control_DisableMPPTTemperatureSwitch()
{
  FOSSASAT_DEBUG_PRINTLN(F("MPPT temperature switching disabled."));
  MPPT_TEMPERATURE_SWITCH_ENABLED = false;
  Power_Control_SaveConfiguration();
}

bool Power_Control_IsMPPTTemperatureSwitchEnabled()
{
  return MPPT_TEMPERATURE_SWITCH_ENABLED;
}


/*
 * @brief the battery will charge fromt he MPPT board and the voltage readings will be the solar panels.
 */
void Power_Control_SwitchOnMPPT()
{
  FOSSASAT_DEBUG_PRINTLN(F("Switching MPPT on."));

  analogWrite(ANALOG_OUT_BATTERY_CHARGING_PIN, HIGH);
}

/*
 * @brief the battery will stop charging (used for sub 0 temperatures)
 */
void Power_Control_SwitchOffMPPT()
{
  FOSSASAT_DEBUG_PRINTLN(F("Switching MPPT off."));

  if (MPPT_KEEP_ALIVE_MODE_ENABLED)
  {
    FOSSASAT_DEBUG_PRINTLN(F("Switching MPPT off disabled."));
    analogWrite(ANALOG_OUT_BATTERY_CHARGING_PIN, HIGH);
    return;
  }

  analogWrite(ANALOG_OUT_BATTERY_CHARGING_PIN, LOW);
}

void Power_Control_CheckBatteryTemperature()
{
  if (!Power_Control_IsMPPTTemperatureSwitchEnabled()) // if the temperature switching is disabled...
  {
    FOSSASAT_DEBUG_PRINTLN("TEmperature switching disabled, MPPT on");

    // keep the mppt ON ie. keep charging the battery.
    Power_Control_SwitchOnMPPT();

    BATTERY_CHARGING_SWITCHED_OFF = false;

    return;
  }

  float batteryTemp = Pin_Interface_GetBatteryTemperature();

  if (batteryTemp < 0.0f) // if low temperature, switch off charging the battery.
  {
    if (BATTERY_CHARGING_SWITCHED_OFF == false)
    {
      Power_Control_SwitchOffMPPT();
      BATTERY_CHARGING_SWITCHED_OFF = true;
    }
  }
  else // if temperature ok, keep charging the battery.
  {
    if (BATTERY_CHARGING_SWITCHED_OFF == true)
    {
      Power_Control_SwitchOnMPPT();
      BATTERY_CHARGING_SWITCHED_OFF = false;
    }
  }
}
