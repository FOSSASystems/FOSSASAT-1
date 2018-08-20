/*
Name:		BeaconTest1.ino
Created:	7/20/2018
Author:	Richad Bamford (FOSSA Systems)
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include <LoRaLib.h>
#include <Wire.h>

#include "configuration.h"
#include "state_machine_declerations.h"
#include "debugging_utilities.h"
#include "persistant_storage.h"
#include "pin_interface.h"
#include "system_info.h"
#include "communication.h"
#include "deployment.h"
    

void setup()
{
	Serial.begin(9600);

  Configuration_SetupPins();

  if (ENABLE_I2C_BUS)
  {
     Wire.begin();
  }

  if (Pin_Interface_ShouldReset())
  {
    Persistant_Storage_Wipe();
  }

  System_Info_IncrementRestartCounter();

	STATE_STARTED = true;

	// Initialize the SX1278 interface with default settings.
	// See the PDF reports on previous PocketQube attempts for more info.
	Debugging_Utilities_DebugPrintLine("SX1278 interface :\nCARRIER_FREQUENCY "
		+ String(CARRIER_FREQUENCY) + " MHz\nBANDWIDTH: "
		+ String(BANDWIDTH) + " kHz\nSPREADING_FACTOR: "
		+ String(SPREADING_FACTOR) + "\nCODING_RATE: "
		+ String(CODING_RATE) + "\nSYNC_WORD: "
		+ String(SYNC_WORD) + "\nOUTPUT_POWER: "
		+ String(OUTPUT_POWER));
    
  MODEM_MODE = MODEM_LORA;
  
	int err_check = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);

	if (err_check == ERR_NONE)
	{
		Debugging_Utilities_DebugPrintLine("(S) SX1278 Online!");
		STATE_TRANSMITTER_INITIALIZED = true;
	}
	else
	{
		// TODO If we reach this, we cannot communicate with the satellite, therefore we need the system to be restarted.
		Debugging_Utilities_DebugPrintLine("(E) SX1278 0x" + String(err_check, HEX));
	}

	Deployment_PowerDeploymentMosfets(); // check or run deployment sequence.
}

/*
 * 
 * 
 * @test Signature malforming during transmission?
 * 
 * @todo Create interval adjustments based on power level.
 */
void loop()
{
  Pin_Interface_WatchdogHeartbeat();

  ///////////////////////////////////////////
  // ENTER RECEIVING MODE FOR LORA PACKETS //
  ///////////////////////////////////////////
  MODEM_MODE = MODEM_LORA;
  int err_check = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);

  if (err_check == ERR_NONE)
  {
    Debugging_Utilities_DebugPrintLine("** (LORA RECEIVE)");
    
    String str;
    byte state = LORA.receive(str);
    
    String signature = str.substring(0, 10);
    String withoutSignature = str.substring(10);
  
    int indexOfS1 = withoutSignature.indexOf(';');
    String message = withoutSignature.substring(indexOfS1 + 1);
  
    String function_id = withoutSignature.substring(0, indexOfS1);
  
    Communication_SX1278Route(function_id, signature, message);
  }
  else
  {
    // TODO If we reach this, we cannot communicate with the satellite, therefore we need the system to be restarted.
    Debugging_Utilities_DebugPrintLine("** (LORA ERROR) SX1278 0x" + String(err_check, HEX));
  }


 
  ///////////////////////////////////////////
  // ENTER RECEIVING MODE FOR FSK PACKETS ///
  ///////////////////////////////////////////
  MODEM_MODE = MODEM_FSK;
  err_check = LORA.beginFSK();
  err_check = LORA.setFrequency(CARRIER_FREQUENCY);
  err_check = LORA.setBitRate(100.0);
  err_check = LORA.setFrequencyDeviation(10.0);
  err_check = LORA.setRxBandwidth(BANDWIDTH);
  err_check = LORA.setOutputPower(OUTPUT_POWER);
  err_check = LORA.setCurrentLimit(100);
  uint8_t syncWord[] = {0x01, 0x23, 0x45, 0x67, 
                        0x89, 0xAB, 0xCD, 0xEF};
  err_check = LORA.setSyncWord(syncWord, 8);
  if (err_check != ERR_NONE)
  {
    Debugging_Utilities_DebugPrintLine("Unable to set configuration, code " + String(err_check, HEX));
  }
  else
  {
    Debugging_Utilities_DebugPrintLine("** (FSK RECEIVE)");
    
    String str;
    byte state = LORA.receive(str);
    
    String signature = str.substring(0, 10);
    String withoutSignature = str.substring(10);
  
    int indexOfS1 = withoutSignature.indexOf(';');
    String message = withoutSignature.substring(indexOfS1 + 1);
  
    String function_id = withoutSignature.substring(0, indexOfS1);
  
    Communication_SX1278Route(function_id, signature, message);
  }
  
  // Payload control.
  if (ENABLE_I2C_BUS)
  {
    delay(200);

    MODEM_MODE = MODEM_LORA;
  
    Wire.requestFrom(8, 32);    // request 32 bytes from slave device #8
  
    String payloadTransmissionMessage = String("");
    
    while (Wire.available())
    { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      payloadTransmissionMessage += String(c);
    }
  
    if (payloadTransmissionMessage != "")
    {
      Communication_SX1278TransmitPayloadMessage(payloadTransmissionMessage);
    }
  }

	delay(1000);
}
