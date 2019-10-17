
/*
Name:		BeaconTest1.ino
Created:	7/20/2018, 06/28/2019
Authors:	Richad Bamford, Jan Gromeš
          FOSSA Systems
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include <RadioLib.h>
#include <Wire.h>
#include <TimerOne.h>

#include "configuration.h"
#include "state_machine_declerations.h"
#include "debugging_utilities.h"
#include "persistant_storage.h"
#include "pin_interface.h"
#include "system_info.h"
#include "communication.h"
#include "deployment.h"
#include "safety_security.h"
#include "power_control.h"
#include "automatic_interval_control.h"
#include "string_functions.h"



#ifdef ENABLE_DEPLOYMENT_SEQUENCE
	bool deploymentOccured = false;
#endif


void changeModem(void)
{
  if(!INTERRUPT_ENABLED)
  {
    return;
  }
  
  // Receive LORA packets.
  // if is not in low power mode
  //    Receive ISM lora packets.
  //    Receive FSK packets.


  if (Power_Control_IsLowPowerModeOperational())
  {
    // Low power mode, force only LORA listening.
    CURRENT_MODEM = 0;
  }

  switch(CURRENT_MODEM)
  {
    case 0:
      Communication_SwitchLORA(false);
      CURRENT_MODEM++;
      break;
    case 1:
      Communication_SwitchLORA_ISM();
      CURRENT_MODEM++;
      break;
    case 2:
      Communication_SwitchFSK();
      CURRENT_MODEM = 0;
      break;
  }

  LORA.startReceive();
}

void receiveInterrupt()
{
  if(!INTERRUPT_ENABLED)
  {
    return;
  }

  RX_FLAG = true;
}





void setup()
{
	FOSSASAT_DEBUG_BEGIN(9600);

  Configuration_SetupPins();

  System_Info_IncrementRestartCounter();
  System_Info_LoadRestartCounter();

  //#define DEV_ENV_CLEAREEPROM
  #ifdef DEV_ENV_CLEAREEPROM
    Persistant_Storage_NonRestoreWipe();
  #endif




  if (Persistant_Storage_Get(EEPROM_FIRST_RUN_ADDR) == 0) // if this is the first run
  {
    // otherwise save the default values.
    Power_Control_SaveConfiguration();

    // save that we have ran the first run
    Persistant_Storage_Set(EEPROM_FIRST_RUN_ADDR, 1);
  }
  else
  {
    // otherwise, load the saved values into RAM.
    Power_Control_LoadConfiguration(); //  load the power configuration
  }



  Power_Control_PrintConfiguration();

	STATE_STARTED = true;

  Pin_Interface_Set_Board_Temperature_Res();
  Pin_Interface_Set_Battery_Temperature_Res();

  #ifdef ENABLE_DEPLOYMENT_SEQUENCE
    deploymentOccured = false;
  #endif

  System_Info_SetCallsign("FOSSASAT-1");


  Communication_SwitchLORA(false);
  LORA.setDio1Action(receiveInterrupt);
  LORA.startReceive();

  Timer1.initialize(MODEM_SWITCHING_TIME);
  Timer1.attachInterrupt(changeModem);
}

/*
 *
 *
 * @test Signature malforming during transmission?
 *
 */
void loop()
{




#ifdef ENABLE_DEPLOYMENT_SEQUENCE
  if (deploymentOccured == false) // RAM check
  {
    // on system restart, if the deployment sequence has already ran, do not deploy again.
    if (Deployment_GetDeploymentState() == 0) // EEPROM check
    {
      // deploy.
      FOSSASAT_DEBUG_PRINTLN(F("Waiting 3 minutes until deploying..."));
      Power_Control_Delay(180000); // wait 3 minutes
      FOSSASAT_DEBUG_PRINTLN(F("Transmitting power information packet..."));

      Communication_SwitchLORA();
      Communication_TransmitPowerInfo(); // transmit the power information.

      FOSSASAT_DEBUG_PRINTLN(F("Done."));
      Power_Control_Delay(300000); // wait 5 minutes.

      FOSSASAT_DEBUG_PRINTLN(F("Deploying..."));
      Deployment_RunSequence(true); // true to ensure EEPROM deployment state checking
      FOSSASAT_DEBUG_PRINTLN(F("Done"));
    }
    else
    {
      // do nothing.
      FOSSASAT_DEBUG_PRINTLN(F("Deploy EEPROM set to 1."));
    }

    deploymentOccured = true; // prevent this loop from running again.
  }
#endif
  if (!STATE_STOP)
  {
    Pin_Interface_WatchdogHeartbeat();
  }
  else
  {
  
    // stop the signal to the heartbeat sensor and wait 10 seconds for a restart instead.
    FOSSASAT_DEBUG_PRINTLN(F("Waiting for watchdog to restart the satellite..."));
    delay(60000); // wait for 1 minute.
  }



  if ( (millis() - LAST_TRANSMIT_STAMP) > TRANSMIT_DELAY)
  {
    // disable timer and receive interrupts to transmit
    INTERRUPT_ENABLED = false;
    detachInterrupt(digitalPinToInterrupt(DIGITAL_PIN_FOR_INTERRUPT));
  

    // transmit system information packet.   //   
    //////////////////
    // NON ISM BAND //
    //////////////////
    Communication_SwitchLORA(false);
  
    ////////////////////////////////
    // TRANSMIT POWER INFORMATION //
    ////////////////////////////////
    if (!Power_Control_IsLowPowerModeOperational())
    {
      FOSSASAT_DEBUG_PRINTLN(F("** (LORA TRANSMIT POWER INFO)"));
      Communication_TransmitPowerInfo();
      FOSSASAT_DEBUG_PRINTLN(F("** (LORA END)"));
    }

    // transmit messages.
    Communication_SX1278RouteTransmit();


    
    // transmission done, enable interrupts, set to correct settings.
    switch(CURRENT_MODEM)
    {
      case 0:
        Communication_SwitchLORA(false);
        break;
      case 1:
        Communication_SwitchLORA_ISM();
        break;
      case 2:
        Communication_SwitchFSK();
        break;
    }
    LORA.setDio1Action(receiveInterrupt);
    LORA.startReceive();
    INTERRUPT_ENABLED = true;

    // update timestamp
    LAST_TRANSMIT_STAMP = millis();
  }





  // Receive transmissions
  if (RX_FLAG)
  {
    INTERRUPT_ENABLED = false;
    RX_FLAG = false;
    
    int state = 0;
    char rxTransmission[MAX_STRING_LENGTH];
    char password[15];
    char functionId[4];
    char message[MAX_STRING_LENGTH];
    
    state = LORA.readData(rxTransmission, MAX_STRING_LENGTH);

    Safety_Security_SX1278ReceptionCodeCheck(state);

    if (state == ERR_NONE)
    {
      FOSSASAT_DEBUG_PRINTLN(rxTransmission);
      
      if (rxTransmission[0] == '5')
      {
        Communication_SX1278RouteReceive("5", "", "", true); // send a manual ping command
      }
      else
      {
        String_Functions_GetPassword(rxTransmission, password);
        bool functionIdOk = String_Functions_GetFunctionId(rxTransmission, functionId);
        FOSSASAT_DEBUG_PRINTLN(functionIdOk);
      
        if (functionIdOk)
        {
          bool messageOk = String_Functions_GetMessage(rxTransmission, message);
          FOSSASAT_DEBUG_PRINTLN(messageOk);
          if (messageOk)
          {
            // parsed transmission is ok for communications router.
            Communication_SX1278RouteReceive(functionId, password, message, false);
          }
        }
      }
    }

    INTERRUPT_ENABLED = true;
  }




  // check the battery temperature every 5 minutes.
  if ( (millis() - LAST_BATTERY_CHECK_STAMP) > BATTERY_CHECK_DELAY)
  {
    Power_Control_CheckBatteryTemperature();
  
    // Low power mode switching
    float batteryVoltageReading = Pin_Interface_GetBatteryVoltage();
    if (batteryVoltageReading <= 3.3f) // if the battery voltage reading is still too low.
    {
      Power_Control_TurnOnLowPowerMode(); // turn on the low power mode (IF ENABLED)
    }
    else
    {
      Power_Control_TurnOffLowPowerMode(); // turn off the lower power mode (IF ENABLED)
    }

    LAST_BATTERY_CHECK_STAMP = millis();
  }




  // every 1 minute, set to sleep (sleep interval is 0 on high battery).
  if ( (millis() - LAST_BATTERY_CHARGE_STAMP) > BATTERY_CHARGE_DELAY)
  {
    Power_Control_Sleep(Automatic_Interval_Control_GetInterval());
    
    LAST_BATTERY_CHARGE_STAMP = millis();
  }
}
