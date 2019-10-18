 /**
 * @file ground_station.ino
 * @brief This code manages the protocol transmission. It takes raw values
 * from the system, packs them into the radio protocol and sends it through the
 * SX1278.
*/

/*
* Name:    GroundStation.ino
* Created:  7/23/2018
* Author: Richad Bamford (FOSSA Systems)
*/
#include <LoRaLib.h>
#include "configuration.h"
#include "debugging_utilities.h"
#include "state_machine_declerations.h"
#include "communication.h"

/* @brief  startup entry point
* @todo catch failure to SX1278 initializing instead of just locking into a while loop().
*/
void setup()
{
  Serial.begin(9600);

  // Initialize the SX1278 interface with default settings.
  // See the PDF reports on previous PocketQube attempts for more info.
  Debugging_Utilities_DebugLog("SX1278 interface :\nCARRIER_FREQUENCY "
  + String(CARRIER_FREQUENCY) + " MHz\nBANDWIDTH: "
  + String(BANDWIDTH) + " kHz\nSPREADING_FACTOR: "
  + String(SPREADING_FACTOR) + "\nCODING_RATE: "
  + String(CODING_RATE) + "\nSYNC_WORD: "
  + String(SYNC_WORD) + "\nOUTPUT_POWER: "
  + String(OUTPUT_POWER));

  byte err_check = LORA.begin(DEFAULT_CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);

  if (err_check == ERR_NONE)
  {
    Debugging_Utilities_DebugLog("(S) SX1278 Online!");
  }
  else
  {
    Debugging_Utilities_DebugLog("(E) SX1278 Error code = 0x" + String(err_check, HEX));
    while (true);
  }
}

void loop()
{
  String str;
  byte state = LORA.receive(str);
  
  String signature = str.substring(0, 10);
  String withoutSignature = str.substring(10);

  int indexOfS1 = withoutSignature.indexOf(';');
  String message = withoutSignature.substring(indexOfS1 + 1);

  String function_id = withoutSignature.substring(0, indexOfS1);

  Serial.println("Received transmission from satellite:");
  Serial.println("  Signature: " + signature);
  Serial.println("  Function ID: " + function_id);
  Serial.println("  Message: " + message);
  Serial.println("End of transmission.");
  
  if (state == ERR_NONE)
  {
      Serial.println("We should receive a log from the comms sub-system...");
    ///////////////
    // recieving //
    ///////////////
    if (function_id == "1")
    {
      Communication_ReceivedStartedSignal();
    }
    if (function_id == "2")
    {
      Communication_ReceivedStoppedSignal();
    }
    if (function_id == "3")
    {
      Communication_ReceivedTransmittedOnline();
    }
    if (function_id == "4")
    {
      Communication_ReceivedDeploymentSuccess();
    }
    if (function_id == "6")
    {
      Communication_ReceivedPong();
    }
    if (function_id == "9")
    {
      Communication_ReceivedPowerInfo(message);
    }
    if (function_id == "10")
    {
      // Get the frequency error given by the LORA lib, to offset the carrier by.
      float frequencyError = LORA.getFrequencyError();

      Communication_ReceivedTune(frequencyError);
    }

    Serial.println("End of comms sub-system. (Error if nothing in this block).");
  }
  else if (state == ERR_RX_TIMEOUT)
  {
    // timeout occurred while waiting for a packet
    Debugging_Utilities_DebugLog("Timeout waiting to receive a packet.");

    if (HAS_REDUCED_BANDWIDTH) // we have found the satellite already, lost connection
    {
      Debugging_Utilities_DebugLog("(DISCONNECTED) Switching back to wide bandwidth mode.");

      CARRIER_FREQUENCY = DEFAULT_CARRIER_FREQUENCY;
      HAS_REDUCED_BANDWIDTH = false; // enable tracking trigger.

      LORA.setFrequency(CARRIER_FREQUENCY); // setup lora for wide bandwidth.
      LORA.setBandwidth(BANDWIDTH);
    }
    else // have not found the satellite already
    {
      Debugging_Utilities_DebugLog("(UNFOUND) Satellite not found! Listening on wide bandwidth mode...");
    }
  }
  else if (state == ERR_CRC_MISMATCH)
  {
    // packet was received, but is malformed
    Debugging_Utilities_DebugLog("Packet has been received but malformed, CRC error,"); 
  }
  else if (state == ERR_UNKNOWN)
  {
    Debugging_Utilities_DebugLog("Unknown error occured!");
  }
  else if (state == ERR_CHIP_NOT_FOUND)
  {
    Debugging_Utilities_DebugLog("Could not find chip.");
  }
  else if (state == ERR_PACKET_TOO_LONG)
  {
    Debugging_Utilities_DebugLog("Packet too long...");
  }
  else if (state == ERR_TX_TIMEOUT)
  {
    Debugging_Utilities_DebugLog("TX Timout.");
  }
  else if (state == ERR_RX_TIMEOUT)
  {
    Debugging_Utilities_DebugLog("RX Timeout.");
  }
  else if (state == ERR_INVALID_BANDWIDTH)
  {
    Debugging_Utilities_DebugLog("Invalid bandwidth.");
  }
  else if (state == ERR_INVALID_SPREADING_FACTOR)
  {
    Debugging_Utilities_DebugLog("Invalid spreading factor.");
  }
  else if (state == ERR_INVALID_CODING_RATE)
  {
    Debugging_Utilities_DebugLog("Invalid coding rate.");
  }
  else if (state == ERR_INVALID_BIT_RANGE)
  {
    Debugging_Utilities_DebugLog("Invalid bit range.");
  }
  else if (state == ERR_INVALID_FREQUENCY)
  {
    Debugging_Utilities_DebugLog("Invalid frequency.");
  }
  else if (state == ERR_INVALID_OUTPUT_POWER)
  {
    Debugging_Utilities_DebugLog("Invalid output power.");
  }
  else if (state == PREAMBLE_DETECTED)
  {
    Debugging_Utilities_DebugLog("Preamble detected.");
  }
  else if (state == CHANNEL_FREE)
  {
    Debugging_Utilities_DebugLog("Channel free.");
  }
  else if (state == ERR_SPI_WRITE_FAILED)
  {
    Debugging_Utilities_DebugLog("SPI write failed");
  }
  else if (state == ERR_INVALID_CURRENT_LIMIT)
  {
    Debugging_Utilities_DebugLog("Invalid current limit");
  }
  else if (state == ERR_INVALID_PREAMBLE_LENGTH)
  {
    Debugging_Utilities_DebugLog("Invalid preamble length");
  }
  else if (state == ERR_INVALID_GAIN)
  {
    Debugging_Utilities_DebugLog("Invalid gain.");
  }
  else
  {
    Debugging_Utilities_DebugLog("ERROR STATE = " + String(state));
  }
}
