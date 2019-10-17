

#include <Arduino.h>
#include <RadioLib.h>

#include "configuration.h"
#include "communication.h"
#include "debugging_utilities.h"
#include "persistant_storage.h"
#include "safety_security.h"
#include "power_control.h"

void Safety_Security_SX1278StartupCheck(int pStartupCode)
{
  if (pStartupCode == ERR_NONE)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 Started correctly"));
  }
  else if (pStartupCode == ERR_CHIP_NOT_FOUND)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 chip not found! Restarting"));

    Power_Control_Delay(15); // delay for 15 minutes

    Communication_RecievedWatchdogStop();
  }
  else
  {
    // unknown error code.
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 Recieved an unknown error code"));
  }
}

void Safety_Security_SX1278TransmissionCodeCheck(int pTransmissionCode)
{
  if (pTransmissionCode == ERR_PACKET_TOO_LONG)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 transmission too long."));
  }
  else if (pTransmissionCode == ERR_TX_TIMEOUT)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 could not transmit correctly, restarting..."));
  }
  else
  {
    // unknown transmission code.
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 transmission code unknown"));
  }
}

void Safety_Security_SX1278ReceptionCodeCheck(int pReceptionCode)
{
  if (pReceptionCode == ERR_RX_TIMEOUT)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 timeout out reading transmission"));
  }
  else if (pReceptionCode == ERR_CRC_MISMATCH)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 CRC mismatch, mismatch"));
  }
  else
  {
    // unknown transmission code.
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 reception code unknown"));
  }
}

/* unused.
void Safety_Security_SX1278ChannelActivityCodeCheck(int pChannelActivityCode)
{
  if (pChannelActivityCode == CHANNEL_FREE)
  {
    // no lora signal detected at this channel
  }
  else if (pChannelActivityCode == PREAMBLE_DETECTED)
  {
    // detected valid LoRa preamble.
  }
}*/

void Safety_Security_SX1278SettingCodeCheck(int pSettingsCode)
{
  if (pSettingsCode == ERR_INVALID_BANDWIDTH)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 settings bandwidth error, restarting."));
  }
  else if (pSettingsCode == ERR_INVALID_SPREADING_FACTOR)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 settings spreading factor error, restarting."));
  }
  else if (pSettingsCode == ERR_INVALID_CODING_RATE)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 settings coding rate error, restarting"));
  }
  else if (pSettingsCode == ERR_INVALID_FREQUENCY)
  {
  }
  else if (pSettingsCode == ERR_INVALID_OUTPUT_POWER)
  {
    FOSSASAT_DEBUG_PRINTLN(F("SX1278 settings output power error."));
  }
  else
  {

    //Debugging_Utilities_DebugPrintLine(F("SX1278 setting configured successfully."));
  }
}

bool Safety_Security_CheckString(char* inString)
{
  // no strings can be above the default 32.
  // protects memory allocation.
  if (strlen(inString) > MAX_STRING_LENGTH)
  {
    return false;
  }

  return true;
}

void Safety_Security_CheckDeploymentFaults(int pPreSoftwareSignal, int pPostSoftwareSignal)
{
  FOSSASAT_DEBUG_PRINTLN(F("Checking deployment for faults..."));

  if (pPreSoftwareSignal == 0 && pPostSoftwareSignal == 0)
  {
    // no change in eeprom address, deployment tried to happen but failed.
    // failed to allocate to eeprom.
    FOSSASAT_DEBUG_PRINTLN(F("Deplyment 0,0"));
    Persistant_Storage_Wipe();
  }
  else if (pPreSoftwareSignal == 0 && pPostSoftwareSignal == 1)
  {
    // change in eeprom address asociated with first startup deployment.
    FOSSASAT_DEBUG_PRINTLN(F("Deplyment 0,1"));
  }
  else if (pPreSoftwareSignal == 1 && pPostSoftwareSignal == 1)
  {
    // no change in eeprom address, associated with manual deploy.
    FOSSASAT_DEBUG_PRINTLN(F("Deplyment 1,1"));
  }
  else if (pPreSoftwareSignal == 1 && pPostSoftwareSignal == 0)
  {
    // failure in eeprom.
    FOSSASAT_DEBUG_PRINTLN(F("Deplyment 1,0"));
    Persistant_Storage_Wipe();
  }

  FOSSASAT_DEBUG_PRINTLN();


  /*
  if (pPreHardwareSignal == 0 && pPostHardwareSignal == 0)
  {
    Debugging_Utilities_DebugPrintLine(F("Restart. No change in deployment hardware signal."));
     // hardware fault.
     // Restart the system.
     Communication_RecievedWatchdogStop();
  }

  if (pPreHardwareSignal == 0 && pPostHardwareSignal == 1)
  {
    // hardware ok. check for eeprom errors.
    Debugging_Utilities_DebugPrintLine(F("Successful deployment and check."));
  }

  if (pPreHardwareSignal == 1 && pPostHardwareSignal == 0)
  {
    // hardware fault.
    // Restart the system.
    Debugging_Utilities_DebugPrintLine(F("Restart. Hardware signal changed from high to low."));

    Communication_RecievedWatchdogStop();
  }

  if (pPreHardwareSignal == 1 && pPostHardwareSignal == 1)
  {
    // deployment already happened
    Debugging_Utilities_DebugPrintLine(F("Deployment has already happened"));
  }
 */
}
