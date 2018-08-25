/**
 * @file communication.cpp
 * @brief This code manages the protocol transmission. It takes raw values
 * from the system, packs them into the radio protocol and sends it through the
 * SX1278.
*/

#include "configuration.h"
#include "state_machine_declerations.h"
#include "pin_interface.h"
#include "persistant_storage.h"
#include "deployment.h"
#include "system_info.h"
#include "communication.h"
#include "debugging_utilities.h"


/*
 * @brief switches SX1278 modem mode to FSK.
 */
int Communication_SwitchFSK()
{
  Debugging_Utilities_DebugPrintLine("(SWITCH FSK MODE)");
  
   //////////////////////
  // FSK MODEM         //
  //////////////////////
  int err_check = LORA.beginFSK();
  return err_check;
}

int Communication_SwitchRTTY()
{
  Debugging_Utilities_DebugPrintLine("(SWITCH RTTY MODE)");
  
  int err_check = LORA.beginFSK();
  LORA.directMode();

  return err_check;
}

int Communication_SwitchLORA()
{
  Debugging_Utilities_DebugPrintLine("(SWITCH LORA MODE)");
  
  ////////////////////////////////
  // LORA MODEM/MODULATION MODE //
  ////////////////////////////////
  int err_check = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);
  return err_check;
}


/////////////////////////////////////////
// TRANSMISSION enabling and disabling //
////////////////////////////////////////
void Communication_DisableTransmitting()
{
  Persistant_Storage_Set(EEPROM_TRANSMISSION_STATE_ADDR, 1);
}

void Communication_EnableTransmitting()
{
  Persistant_Storage_Set(EEPROM_TRANSMISSION_STATE_ADDR, 2);
}

bool Communication_GetTransmittingState()
{
  int transmissionState = Persistant_Storage_Get(EEPROM_TRANSMISSION_STATE_ADDR);

  if (transmissionState == 1)
  {
    return false; // transmission is disabled at the value of 1.
  }
  else if (transmissionState == 2)
  {
    return true; // transmission is enabled at the value of 2.
  }
  else if (transmissionState == 0)
  {
    return true; // an undefined transmission state is enabled.
  }
  else 
  {
    return true; // if eeprom is broken.
  }
}

//////////////////////////////////
// RTTY Protocol implementation //
//////////////////////////////////
void Communication_RTTY_TransmitMark()
{
  unsigned long start = micros(); // MICRO seconds.
  
  tone(DIGITAL_OUT_SX1278_DIRECT, RTTY_BASE - RTTY_SHIFT);
  
  // Delay for sub millisecond times.
  while (micros() - start < RTTY_BAUD_RATE);

  noTone(DIGITAL_OUT_SX1278_DIRECT);
}

void Communication_RTTY_TransmitSpace()
{
  unsigned long start = micros(); // MICRO seconds.
  
  tone(DIGITAL_OUT_SX1278_DIRECT, RTTY_BASE);
  
  while (micros() - start < RTTY_BAUD_RATE);

  noTone(DIGITAL_OUT_SX1278_DIRECT);
}

void Communication_RTTY_TransmitBit(char inChar)
{
  // for each bit in char (8-bits);
  char m = 0x01; // mask
  for (m; m; m <<= 1)
  {
    // if the char bit is selected by the mask...
    if (inChar & m)
    {
      // send a 1 (MARK) = base + shift
      Communication_RTTY_TransmitMark();
    }
    else
    {
      // send a 0 (SPACE) = base
      Communication_RTTY_TransmitSpace();
    }
  }
}

/*
 * @brief send a string through the RTTY protocol.
 * 
 * @todo check if the \r\n chars are being transmitted.
 */
void Communication_RTTY_Transmit(String inTransmissionPacket)
{
  // for each 8-bit byte/char.
  for (int i = 0; i < inTransmissionPacket.length(); i++)
  {
    Communication_RTTY_TransmitBit(inTransmissionPacket.c_str()[i]);
  }
}

void Communication_RTTY_BeginTransmission()
{
    Communication_RTTY_TransmitMark();
    delay(500);
}

void Communication_RTTY_EndTransmission()
{   
    Communication_RTTY_TransmitSpace();
    Communication_RTTY_TransmitMark();
    LORA.packetMode();
}


/*
 * @brief Routes the received string infomation to the internal functions.
 * inMessage is always empty since we are sending no data to the satellite.
 */
void Communication_SX1278Route(String inFunctionId, String inSignature, String inMessage)
{
    /*
  Serial.println("Signature: " + String(signature));
  Serial.println("Function ID: " + functionId);
  Serial.println("Message: " + message);
   */
               
  if (System_Info_CheckSystemSignature(inSignature) == false) // invalid signature
  {
    Debugging_Utilities_DebugPrintLine("(SIG MISMATCH)");
  }
   
  bool transmittingState = Communication_GetTransmittingState();
  
  if (transmittingState == true) // cirital decision, transmission recieved to turn off transmission is REQUIRED and stored in EEPROM.
  {  
    Debugging_Utilities_DebugPrintLine("(COMMS SYS START)");
    
    ///////////////
    // RECIEVING //
    ///////////////
    if (inFunctionId == "5")
    {
      Communication_RecievedPing();
    }
    if (inFunctionId == "7")
    {
      Communication_RecievedStopTransmitting(); // switch the TRANSMISSION_ENABLED boolean.
    }


    ////////////////////////////
    // TRANSMITTING TO GROUND //
    ////////////////////////////
    if (STATE_STARTED)
    {
      Communication_TransmitStartedSignal();
      STATE_STARTED = false;
    }
    if (STATE_STOPPED)
    {
      Communication_TransmitStoppedSignal();
      STATE_STOPPED = false;
    }
    if (STATE_TRANSMITTER_INITIALIZED)
    {
      Communication_TransmitSX1278InitializedSuccess();
      STATE_TRANSMITTER_INITIALIZED = false;
    }
    if (STATE_DEPLOYMENT_SUCCESS)
    {
      Communication_TransmitDeploymentSuccess();
      STATE_DEPLOYMENT_SUCCESS = false;
    }
    if (STATE_PING)
    {
      Communication_TransmitPong();
      STATE_PING = false;
    }
    if (STATE_TRANSMIT_POWER_INFO)
    {
      Communication_TransmitPowerInfo();
      STATE_TRANSMIT_POWER_INFO = false;
    }
    if (STATE_TRANSMIT_TUNE)
    {
       Communication_TransmitTune();
       STATE_TRANSMIT_TUNE = false;
    }

    POWER_INFO_TIMER = POWER_INFO_TIMER + 1;
    if (POWER_INFO_TIMER >= POWER_INFO_DELAY)
    {
      POWER_INFO_TIMER = 0;
      STATE_TRANSMIT_POWER_INFO = true;
    }
    
    TUNE_TIMER = TUNE_TIMER + 1;
    if (TUNE_TIMER >= TUNE_TIMER_DELAY)
    {
      TUNE_TIMER = 0;
      STATE_TRANSMIT_TUNE = true;
    }
      
    Debugging_Utilities_DebugPrintLine("(COMM SYSTEM END");
  }
  else
  {
    Debugging_Utilities_DebugPrintLine("(TRANS. DISAB. MODE)");
    
    if (inFunctionId == "8") // only parse the function id of 8.
    {
      Communication_RecievedStartTransmitting();
    }
    
    Debugging_Utilities_DebugPrintLine("(TRANS. DISAB. MODE END");
  }
}

/* @brief abstraction for the LORA.transmit function.
 * 
 */
void Communication_SX1278TransmitPacket(String inTransmissionPacket)
{
  bool transmittingState = Communication_GetTransmittingState();
  
  if (transmittingState == true)
  {
    Debugging_Utilities_DebugPrintLine("(Transmit) " + inTransmissionPacket);
    
    int state = LORA.transmit(inTransmissionPacket);
    
    Debugging_Utilities_DebugPrintLine("state = " + String(state));
    
    if (state == ERR_NONE)
    {
      // the packet was successfully transmitted
      Debugging_Utilities_DebugPrintLine("(Sent packet)");
    }
    else if (state == ERR_PACKET_TOO_LONG)
    {
      // the supplied packet was longer than 256 bytes
      Debugging_Utilities_DebugPrintLine("(Packet too big)");
    }
    else if (state == ERR_TX_TIMEOUT)
    {
      // timeout occurred while transmitting packet
      Debugging_Utilities_DebugPrintLine("(Packet timeout)");
    }  
  }
  else
  {
      Debugging_Utilities_DebugPrintLine("(Trans. Disab.)");
  }
}
/**
 * @brief The main transmission function.
 * @param inFuncId The number that represents the function being sent.
 * @param inMessage The string that will be transmistted to the satellite.
 * 
 * This should never be called directly in the main code body. There are
 * more specific functions that handle custom functionality.
 * 
 * @todo Debug Logging removal. 
 * @todo Transmission signature containment.
 * @todo check transmission packet byte length is below 63 bytes for FSK.
 * 
 * @test inFuncId range String(-inf) to String(inf).
 * @test inFuncId String length above 256 
 * @test InFuncId empty string.
 * @test inMessage String empty.
 * @test inMessage length above 256.
 */
void Communication_SX1278Transmit(String inFuncId, String inMessage)
{
  // create transmission packet.
  String transmissionSignature = System_Info_GetTransmissionSignature();
  String transmissionPacket = transmissionSignature + inFuncId + ";" + inMessage;

  ///////////////////////
  // LORA Transmission //
  ///////////////////////
  int err_check = Communication_SwitchLORA();
  if (err_check == ERR_NONE)
  {
    Debugging_Utilities_DebugPrintLine("(LORA MODE)");
    Communication_SX1278TransmitPacket(transmissionPacket);
    Debugging_Utilities_DebugPrintLine("(LORA S.");
  }
  else
  {
    // TODO If we reach this, we cannot communicate with the satellite, therefore we need the system to be restarted.
    Debugging_Utilities_DebugPrintLine("(LORA MODE E) SX1278 0x" + String(err_check, HEX));
  }

  delay(100);

  ///////////////////////
  // FSK Transmission //
  //////////////////////
  err_check = Communication_SwitchFSK();
  if (err_check != ERR_NONE)
  {
    Debugging_Utilities_DebugPrintLine("Unable to set configuration, code " + String(err_check, HEX));
  }
  else
  {
    Debugging_Utilities_DebugPrintLine("(FSK MODE)");
    Communication_SX1278TransmitPacket(transmissionPacket);
    Debugging_Utilities_DebugPrintLine("(FSK S.");
  }

  delay(100);

  ////////////////////////
  // RTTY Transmission //
  ///////////////////////
  err_check = Communication_SwitchRTTY();
  if (err_check != ERR_NONE)
  {
    Debugging_Utilities_DebugPrintLine("Unable to set configuration, code " + String(err_check, HEX));
  }
  else
  {
    Debugging_Utilities_DebugPrintLine("(RTTY MODE)");
    
    Communication_RTTY_BeginTransmission();
    Communication_RTTY_Transmit(transmissionPacket);
    Communication_RTTY_EndTransmission();
  
    Debugging_Utilities_DebugPrintLine("(T. RTTY. Success)");
  }
 
}

 /*
 * @brief an abstraction layer for the payload transmissions.
 */
void Communication_SX1278TransmitPayloadMessage(String inPayloadMessage)
{
  int indexOfSeperator = inPayloadMessage.indexOf(';'); // first ; is the seperator between funcid and message data.
  String message = inPayloadMessage.substring(indexOfSeperator + 1); // from after the first ; to the end
  String functionId = inPayloadMessage.substring(0, indexOfSeperator);  // from the start to the first ; (exluding);

  Communication_SX1278Transmit(functionId, message);
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitStartedSignal()
{
  Debugging_Utilities_DebugPrintLine("(T. Start. Trans.)");
	Communication_SX1278Transmit("1", "");
}

/**
 * @brief satellite's failure sequency function is called..
 *
 * This function indicates to the ground station that the satellite has stopped.
 */
void Communication_TransmitStoppedSignal()
{
  Debugging_Utilities_DebugPrintLine("(T. Stop. Trans.)");
	Communication_SX1278Transmit("2", "");
}

/**
 * @brief satellite's SX1278 LoRa library .begin() ran successfully.
 *
 * This function indicates to the ground station that the satellite has began the LoRa object successfully.
 */
void Communication_TransmitSX1278InitializedSuccess()
{
  Debugging_Utilities_DebugPrintLine("(T. SX1278 S.");
	Communication_SX1278Transmit("3", "");
}

/**
 * @brief When the Atmega setup() is called.
 *
 * This function indicates to the ground station that the satellite has deployed successfully.
 * @todo !!! How do we handle missing packets? we could miss a deployment success and assume it has failed?
 */
void Communication_TransmitDeploymentSuccess()
{
  Debugging_Utilities_DebugPrintLine("(T. Dep. S.)");
	Communication_SX1278Transmit("4", "");
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitPong()
{
  Debugging_Utilities_DebugPrintLine("(T. Pong)");
	Communication_SX1278Transmit("6", "");
}

/**
 * @brief Satellite PING transmission received.
 *
 * This function is to test the connection using a minimal packet.
 */
void Communication_RecievedPing()
{
  Debugging_Utilities_DebugPrintLine("(R. Ping)");
	STATE_PING = true;
}

/**
 * @brief Satellite Stop transmission.
 *
 * This prevents the satellite from transmitting and configures it to only listen to start transmitting messages.
 */
void Communication_RecievedStopTransmitting()
{
  Debugging_Utilities_DebugPrintLine("(R. Stop. T");
	Communication_DisableTransmitting();
}

/**
 * @brief Satellite Start transmission.
 *
 * This re-enables transmitting and receiving for all routes.
 */
void Communication_RecievedStartTransmitting()
{
  Debugging_Utilities_DebugPrintLine("(R. Start. T)");
	Communication_EnableTransmitting();
}

/**
 * @brief Power infomation send to ground station every 400ms.
 *
 * Collects Solar cell voltages, deployment state, reset count and batter charging current
 * then transmits it to the ground station.
 */
void Communication_TransmitPowerInfo()
{
  Debugging_Utilities_DebugPrintLine("(T. Sys. Info)");
  
  String batteryChargingVoltage = Pin_Interface_GetBatteryChargingVoltage();
  String batteryVoltage = Pin_Interface_GetBatteryVoltage();
  String totalSolarCellVoltage = Pin_Interface_GetTotalSolarCellVoltage();
  
	String deploymentState = Deployment_GetDeploymentState();
	String resetCounter = System_Info_GetResetCounter();

	String sysInfoMessage = String("");
	sysInfoMessage += String("BC:") + batteryChargingVoltage + ";";
  sysInfoMessage += String("B:") + batteryVoltage + ";";
  sysInfoMessage += String("TS:") + totalSolarCellVoltage + ";";
  sysInfoMessage += String("RC:") + resetCounter + ";";
  sysInfoMessage += String("DS:") + deploymentState + ";";

	Communication_SX1278Transmit("9", sysInfoMessage);
}

/**
 * @brief Satellite's ground station TUNE transmission.
 *
 * This is send every 1s which the ground station tunes its frequency to
 */
void Communication_TransmitTune()
{
  Debugging_Utilities_DebugPrintLine("(T. TUNE)");
  
	// switch to wide bandwidth location transmission.
	LORA.setBandwidth(LOCATION_BANDWIDTH);

	Communication_SX1278Transmit("10", String("FOSSASAT1"));

	// return to local bandwidth transmissions.
	LORA.setBandwidth(BANDWIDTH);
 
	Debugging_Utilities_DebugPrintLine("(E. T. TUNE)");
}
