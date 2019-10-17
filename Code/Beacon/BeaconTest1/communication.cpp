 /*
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
#include "power_control.h"
#include "safety_security.h"
#include "string_functions.h"

/* @brief the buffer for relay messages starts empty, ready for the first read.
 */
char COMMUNICATION_REPEATER_MESSAGE[MAX_STRING_LENGTH];

/*
 * @brief switches SX1278 modem mode to FSK.
 */
void Communication_SwitchFSK()
{
  int err, startupErr;

  startupErr = LORA.beginFSK();
  Safety_Security_SX1278StartupCheck(startupErr);

  err = LORA.setFrequency(FSK_CARRIER_FREQUENCY);
  Safety_Security_SX1278SettingCodeCheck(err);

  err = LORA.setBitRate(FSK_BIT_RATE);
  Safety_Security_SX1278SettingCodeCheck(err);

  err = LORA.setFrequencyDeviation(FSK_FREQUENCY_DERIVATION);
  Safety_Security_SX1278SettingCodeCheck(err);

  err = LORA.setRxBandwidth(FSK_RX_BANDWIDTH);
  Safety_Security_SX1278SettingCodeCheck(err);

  err = LORA.setOutputPower(FSK_OUTPUT_POWER);
  Safety_Security_SX1278SettingCodeCheck(err);

  err = LORA.setCurrentLimit(FSK_CURRENT_LIMIT);
  Safety_Security_SX1278SettingCodeCheck(err);

  err = LORA.setDataShaping(FSK_DATA_SHAPING);
  Safety_Security_SX1278SettingCodeCheck(err);

  uint8_t syncWord[] = { SYNC_WORD_A, SYNC_WORD_B };
  err = LORA.setSyncWord(syncWord, 2);
  Safety_Security_SX1278SettingCodeCheck(err);

  Communication_SetTXC();
}

void Communication_SwitchLORA(bool inLSFMode)
{
  uint8_t syncWord[] = { SYNC_WORD_A, SYNC_WORD_B };
  int err;
  if (!inLSFMode) // not in lower spreading factor mode
  {
    if (SATELLITE_ALTERNATIVE_SF_MODE) // use the alternative SF mode?
    {
      DEBUG_PRINTLN(F("LORA alternative mode"));
      err = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, ALTERNATIVE_SPREADING_FACTOR, CODING_RATE, syncWord, OUTPUT_POWER, CURRENT_LIMIT);
    }
    else
    {
      DEBUG_PRINTLN(F("LORA non-alternative mode"));
      err = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, syncWord, OUTPUT_POWER, CURRENT_LIMIT);
    }
  }
  else
  {
    DEBUG_PRINTLN(F("LORA LSF mode"));
    err = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, LOWER_SPREADING_FACTOR, CODING_RATE, syncWord, OUTPUT_POWER, CURRENT_LIMIT);
  }

  Communication_SetTXC();
  Safety_Security_SX1278StartupCheck(err);
}

void Communication_SwitchLORA_ISM()
{
  uint8_t syncWord[] = { SYNC_WORD_A, SYNC_WORD_B };
  int err;
  if (SATELLITE_ALTERNATIVE_SF_MODE) // use the alternative SF mode?
  {
    DEBUG_PRINTLN(F("LORA ISM alternative mode"));
    err = LORA.begin(ISM_CARRIER_FREQUENCY, ISM_BANDWIDTH, ISM_ALTERNATIVE_SPREADING_FACTOR, ISM_CODING_RATE, syncWord, ISM_OUTPUT_POWER, ISM_CURRENT_LIMIT);
  }
  else
  {

   DEBUG_PRINTLN(F("LORA ISM non-alternative mode"));
    err = LORA.begin(ISM_CARRIER_FREQUENCY, ISM_BANDWIDTH, ISM_SPREADING_FACTOR, ISM_CODING_RATE, syncWord, ISM_OUTPUT_POWER, ISM_CURRENT_LIMIT);
  }

  Communication_SetTXC();
  Safety_Security_SX1278StartupCheck(err);
}


void Communication_SwitchRTTY()
{
  int startupErr, err;

  startupErr = LORA.beginFSK();
  err = LORA.setFrequency(CARRIER_FREQUENCY);

  Communication_SetTXC();

  Safety_Security_SX1278StartupCheck(startupErr);

  RTTY.begin(CARRIER_FREQUENCY, RTTY_FREQUENCY_SHIFT, RTTY_BAUDRATE);
}


/*
 * @brief Configures the temperature controlled crystal
 */
 void Communication_SetTXC()
 {
    // enable TCXO
    // reference voltage:           1.6 V
    // timeout:                     5000 us
    int state = LORA.setTCXO(TXC_REF_VOLTAGE);
    if (state == ERR_NONE)
    {
    }
    else
    {
      DEBUG_PRINT(F("TCXO failed, code "));
      DEBUG_PRINTLN(state);
      while (true);
    }
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

/*
 * @brief Routes the received string infomation to the internal functions.
 * inMessage is always empty since we are sending no data to the satellite.
 *
 * @todo validate the inMessage string...
 */
void Communication_SX1278RouteReceive(char* inFunctionId, char* inPassword, char* inMessage, bool inPingTransmission)
{
  DEBUG_PRINT("Password: ");
  DEBUG_PRINTLN(inPassword);

  DEBUG_PRINT("Function ID: ");
  DEBUG_PRINTLN(inFunctionId);

  DEBUG_PRINT("Message: ");
  DEBUG_PRINTLN(inMessage);

  DEBUG_PRINT("Is a PING transmission?: " );
  DEBUG_PRINTLN((char)inPingTransmission);

  if (strcmp(inFunctionId, "") == 0)
  {
    DEBUG_PRINTLN(F("RECEIVED EMPTY VALUES AS PARAMS"));
    return;
  }

  bool transmittingState = Communication_GetTransmittingState();

  // cirital decision, transmission recieved to turn off transmission is REQUIRED and stored in EEPROM.
  if (transmittingState == true)
  {
    ///////////////
    // RECIEVING //
    ///////////////
    if (inPingTransmission) // if we received a ping, don't do any other receive functions.
    {
      // ignore the password and receive the ping command
      Communication_RecievedPing();
    }
    else
    {
      // ensure the transmissions start with the password
      if (strcmp(inPassword, System_Info_GetTransmissionPassword()) == 0)
      {
        if (Power_Control_IsLowPowerModeOperational())
        {
          // restricted set
          if (strcmp(inFunctionId, "7") == 0)
          {
            Communication_RecievedStopTransmitting(); // switch the TRANSMISSION_ENABLED boolean off.
          }
          else if (strcmp(inFunctionId, "21") == 0)
          {
            Communication_ReceivedEnableMPPTTemperatureSwitching();
          }
          else if (strcmp(inFunctionId, "22") == 0)
          {
            Communication_ReceivedDisableMPPTTemperatureSwitching();
          }
          else if (strcmp(inFunctionId, "23") == 0)
          {
            Communication_ReceivedEnableLowPowerMode();
          }
          else if (strcmp(inFunctionId, "24") == 0)
          {
            Communication_ReceivedDisableLowPowerMode();
          }
          else if (strcmp(inFunctionId, "25") == 0)
          {
            Communication_ReceivedManualSystemInformationPacketSend();
          }
          else if (strcmp(inFunctionId, "26") == 0)
          {
            Communication_ReceivedMPPTKeepAliveEnable();
          }
          else if (strcmp(inFunctionId, "27") == 0)
          {
            Communication_ReceivedMPPTKeepAliveDisable();
          }
        }
        else
        {
          if (strcmp(inFunctionId, "5") == 0)
          {
            Communication_RecievedPing();
          }
          else if (strcmp(inFunctionId, "7") == 0)
          {
            Communication_RecievedStopTransmitting(); // switch the TRANSMISSION_ENABLED boolean off.
          }
          else if (strcmp(inFunctionId, "10") == 0)
          {
            Communication_RecievedResetEEPROM();
          }
          else if (strcmp(inFunctionId, "11") == 0)
          {
             Communication_RecievedDeployment();
          }
          else if (strcmp(inFunctionId, "13") == 0)
          {
             Communication_RecievedCallsignChange(inMessage);
          }
          else if (strcmp(inFunctionId, "14") == 0)
          {
            Communication_RecievedWatchdogStop();
          }
          else if (strcmp(inFunctionId, "15") == 0)
          {
            Communication_RecievedRepeaterMessage(inMessage);
          }
          else if (strcmp(inFunctionId, "17") == 0)
          {
            Communication_RecievedSendLSFCallsign();
          }
          else if (strcmp(inFunctionId, "19") == 0)
          {
            Communication_RecievedEnableAlternativeSF();
          }
          else if (strcmp(inFunctionId, "20") == 0)
          {
            Communication_RecievedDisabledAlternativeSF();
          }
          else if (strcmp(inFunctionId, "21") == 0)
          {
            Communication_ReceivedEnableMPPTTemperatureSwitching();
          }
          else if (strcmp(inFunctionId, "22") == 0)
          {
            Communication_ReceivedDisableMPPTTemperatureSwitching();
          }
          else if (strcmp(inFunctionId, "23") == 0)
          {
            Communication_ReceivedEnableLowPowerMode();
          }
          else if (strcmp(inFunctionId, "24") == 0)
          {
            Communication_ReceivedDisableLowPowerMode();
          }
          else if (strcmp(inFunctionId, "25") == 0)
          {
            Communication_ReceivedManualSystemInformationPacketSend();
          }
          else if (strcmp(inFunctionId, "26") == 0)
          {
            Communication_ReceivedMPPTKeepAliveEnable();
          }
          else if (strcmp(inFunctionId, "27") == 0)
          {
            Communication_ReceivedMPPTKeepAliveDisable();
          }
        }
      }
      else
      {
        DEBUG_PRINTLN(F("Password incorrect."));
      }
    }
  }
  else // in disabled transmission mode.
  {
    DEBUG_PRINTLN(F("TRANSMISSION DISABLED"));

    // ensure the transmissions start with the password even when transmissions disabled.
    if (strcmp(inPassword, System_Info_GetTransmissionPassword()) == 0)
    {
      if (strcmp(inFunctionId, "8") == 0) // only parse the function id of 8 to turn transmitting on.
      {
        Communication_RecievedStartTransmitting();
      }
    }
    else
    {
      DEBUG_PRINTLN(F("Password incorrect."));
    }
  }
}

void Communication_SX1278RouteTransmit()
{
  bool transmittingState = Communication_GetTransmittingState();

  // cirital decision, transmission recieved to turn off transmission is REQUIRED and stored in EEPROM.
  if (transmittingState == true)
  {
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
    if (STATE_TRANSMIT_REPEATER_MESSAGE)
    {
      Communication_TransmitRepeaterMessage();
      STATE_TRANSMIT_REPEATER_MESSAGE = false;
    }
    if (STATE_STOP)
    {
      STOPPED_COUNTER++;

      if (STOPPED_COUNTER >= STOPPED_COUNTER_MAX)
      {
        /*
         * todo at the moment we turn the watchdog pulse back on if it fails to stop the beacon.
         */
         STOPPED_COUNTER = 0;
         STATE_STOP = false; // see line 89 in BeaconTest1.cpp (disables watchdog signal at the start of the loop)
      }
    }
    if (STATE_TRANSMIT_LSF_CALLSIGN)
    {
      Communication_TransmitLSFCallsign();
      STATE_TRANSMIT_LSF_CALLSIGN = false;
    }
    if (STATE_MANUAL_TRANSMIT_SYTEM_INFORMATION)
    {
      Communication_TransmitPowerInfo();
      STATE_MANUAL_TRANSMIT_SYTEM_INFORMATION = false;
    }

    RTTY_CALLSIGN_TRANSMIT_TIMER = RTTY_CALLSIGN_TRANSMIT_TIMER + 1;
    if (RTTY_CALLSIGN_TRANSMIT_TIMER >= RTTY_CALLSIGN_TRANSMIT_DELAY)
    {
      RTTY_CALLSIGN_TRANSMIT_TIMER = 0;
      STATE_TRANSMIT_RTTY_CALLSIGN = true;
    }
  }
  else // in disabled transmission mode.
  {
  }
}

/* @brief abstraction for the LORA.transmit function.
 *
 */
void Communication_SX1278TransmitPacket(char* inTransmissionPacket)
{
  bool transmittingState = Communication_GetTransmittingState();

  if (transmittingState == true)
  {
    DEBUG_PRINTLN(F("Transmitting: "));
    DEBUG_PRINTLN(inTransmissionPacket);

    for (int i = 0; i < TRANSMISSION_REPETITION_COUNT; i++)
    {
      int state = LORA.transmit(inTransmissionPacket);
      Safety_Security_SX1278TransmissionCodeCheck(state);
    }

    DEBUG_PRINTLN(F("Transmitted"));
  }
  else
  {
    DEBUG_PRINTLN("TRANSMISSION DISABLED");
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
 * @todo check transmission packet byte length is below 63 bytes for FSK. - FIXED (02/01/2019) no longer require FSK transmissions.
 *
 * @test inFuncId range String(-inf) to String(inf).
 * @test inFuncId String length above 256
 * @test InFuncId empty string.
 * @test inMessage String empty.
 * @test inMessage length above 256.
 */
void Communication_SX1278Transmit(char* inFuncId, char* inMessage, bool inLSFMode)
{
  // create transmission packet.
  char* transmissionCallsign = System_Info_GetCallsign();

  char transmissionPacket[MAX_STRING_LENGTH * 2]; // 64 bytes
  strcpy(transmissionPacket, transmissionCallsign);
  strcat(transmissionPacket, inFuncId);
  strcat(transmissionPacket, ";");
  strcat(transmissionPacket, inMessage);

  ///////////////////////
  // LORA Transmission //
  ///////////////////////
  Communication_SwitchLORA(inLSFMode);
  Communication_SX1278TransmitPacket(transmissionPacket);

  Power_Control_Delay(500);

  // 27/11/2018 FSK is uplink only.

  ////////////////////////
  // RTTY Transmission //
  ///////////////////////
  if (ENABLE_RTTY)
  {
    if (STATE_TRANSMIT_RTTY_CALLSIGN)
    {
      Communication_SwitchRTTY();

      DEBUG_PRINTLN(F("TRANSMITTING RTTY..."));

      RTTY.idle();
      Power_Control_Delay(500); // can we remove that?

      RTTY.println(transmissionPacket);

      DEBUG_PRINTLN(F("DONE"));

      STATE_TRANSMIT_RTTY_CALLSIGN = false;
    }
  }

  Power_Control_Delay(50);
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitStartedSignal()
{
  Communication_SX1278Transmit("1", "");
}

/**
 * @brief satellite's failure sequency function is called..
 *
 * This function indicates to the ground station that the satellite has stopped.
 */
void Communication_TransmitStoppedSignal()
{
  Communication_SX1278Transmit("2", "");
}

/**
 * @brief When the Atmega setup() is called.
 *
 * This function indicates to the ground station that the satellite has deployed successfully.
 */
void Communication_TransmitDeploymentSuccess()
{
  Communication_SX1278Transmit("4", "");
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitPong()
{
  Communication_SX1278Transmit("6", "");
}

/**
 * @brief Satellite PING transmission received.
 *
 * This function is to test the connection using a minimal packet.
 */
void Communication_RecievedPing()
{
  STATE_PING = true;
}

/**
 * @brief Satellite Stop transmission.
 *
 * This prevents the satellite from transmitting and configures it to only listen to start transmitting messages.
 */
void Communication_RecievedStopTransmitting()
{
  Communication_DisableTransmitting();
}

/**
 * @brief Satellite Start transmission.
 *
 * This re-enables transmitting and receiving for all routes.
 */
void Communication_RecievedStartTransmitting()
{
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
  float batteryChargingVoltage = Pin_Interface_GetBatteryChargingVoltage();
  float batteryVoltage = Pin_Interface_GetBatteryVoltage();
  float solarCellAVoltage = Pin_Interface_GetSolarCellAVoltage();
  float solarCellBVoltage = Pin_Interface_GetSolarCellBVoltage();
  float solarCellCVoltage = Pin_Interface_GetSolarCellCVoltage();

  int deploymentState = Deployment_GetDeploymentState();
  int resetCounter = System_Info_GetResetCounter();

  int lowPowerMode = 0;
  if (Power_Control_IsLowPowerModeOperational())
  {
    lowPowerMode = 1;
  }

  float boardTemperature = Pin_Interface_GetBoardTemperature();

  char floatValueContainer[10];
  char intValueContainer[4];
  char* seperator = ",";

  char systemInformationMessage[MAX_STRING_LENGTH * 2]; // 64 bytes for data alone.

  String_Functions_IntegerToString(intValueContainer, lowPowerMode);
  strcpy(systemInformationMessage, intValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_FloatToString(floatValueContainer, batteryVoltage);
  strcat(systemInformationMessage, floatValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_FloatToString(floatValueContainer, solarCellAVoltage);
  strcat(systemInformationMessage, floatValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_FloatToString(floatValueContainer, solarCellBVoltage);
  strcat(systemInformationMessage, floatValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_FloatToString(floatValueContainer, solarCellCVoltage);
  strcat(systemInformationMessage, floatValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_IntegerToString(intValueContainer, deploymentState);
  strcat(systemInformationMessage, intValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_IntegerToString(intValueContainer, resetCounter);
  strcat(systemInformationMessage, intValueContainer);

  strcat(systemInformationMessage, seperator);

  String_Functions_FloatToString(floatValueContainer, boardTemperature);
  strcat(systemInformationMessage, floatValueContainer);

  Communication_SX1278Transmit("9", systemInformationMessage);
}

// 10
void Communication_RecievedResetEEPROM()
{
   Persistant_Storage_Wipe();
}

// 11
void Communication_RecievedDeployment()
{
   Deployment_RunSequence(false);
}

//12

//13
void Communication_RecievedCallsignChange(char* inMessage)
{
  if (!Safety_Security_CheckString(inMessage))
  {
    return;
  }

   System_Info_SetCallsign(inMessage);
   Power_Control_Delay(1000); // wait for a second.
}

// 14
void Communication_RecievedWatchdogStop()
{
    STATE_STOPPED = true; // makes the satellite send a stopped transmission.
    STATE_STOP = true;
}

// 15 received repeater message
void Communication_RecievedRepeaterMessage(char* inMessage)
{
  // ensure that the repeater message is safe for storing in memory.
  if (!Safety_Security_CheckString(inMessage))
  {
    String_Functions_Zero(COMMUNICATION_REPEATER_MESSAGE);
    return;
  }

  strcpy(COMMUNICATION_REPEATER_MESSAGE, inMessage);

  DEBUG_PRINTLN(F("_________________RECEIVED REPEATER MESSAGE"));

  STATE_TRANSMIT_REPEATER_MESSAGE = true;
}

// 16 transmit repeater message.
void Communication_TransmitRepeaterMessage()
{
  if (!Safety_Security_CheckString(COMMUNICATION_REPEATER_MESSAGE))
  {
    String_Functions_Zero(COMMUNICATION_REPEATER_MESSAGE);
    return;
  }

  Communication_SX1278Transmit("16", COMMUNICATION_REPEATER_MESSAGE);
  String_Functions_Zero(COMMUNICATION_REPEATER_MESSAGE);
}

// 17 received LSF command
void Communication_RecievedSendLSFCallsign()
{
  STATE_TRANSMIT_LSF_CALLSIGN = true;
}

// 18 transmit LSF
void Communication_TransmitLSFCallsign()
{
  Communication_SX1278Transmit("18", "", true);
}


// 19 switch to alternative spreading factor
void Communication_RecievedEnableAlternativeSF()
{
  SATELLITE_ALTERNATIVE_SF_MODE = true;
}

// 20 switch to regular spreading factor.
void Communication_RecievedDisabledAlternativeSF()
{
  SATELLITE_ALTERNATIVE_SF_MODE = false;
}

// 21 enable MPPT charging circuit.
void Communication_ReceivedEnableMPPTTemperatureSwitching()
{
  Power_Control_EnableMPPTTemperatureSwitch();
}

// 22 disable mppt charging circuit.
void Communication_ReceivedDisableMPPTTemperatureSwitching()
{
  Power_Control_DisableMPPTTemperatureSwitch();
}

// 23 enable low power mode system.
void Communication_ReceivedEnableLowPowerMode()
{
  Power_Control_EnableLowPowerMode(); // now the low power mode will be turned on when the battery goes below 3.3v
}

// 24 disable low power mode system.
void Communication_ReceivedDisableLowPowerMode()
{
  Power_Control_TurnOffLowPowerMode(); // force switch to normal mode.

  Power_Control_DisableLowPowerMode(); // now the low power mode will be ignored.
}

// 25 system information packet manual send command.
void Communication_ReceivedManualSystemInformationPacketSend()
{
  STATE_MANUAL_TRANSMIT_SYTEM_INFORMATION = true;
}


// 26 - enable mppt keep alive, stops the mppt from being disabled.
void Communication_ReceivedMPPTKeepAliveEnable()
{
  Power_Control_TurnOnMPPTKeepAlive();
}

// 27 - disabled mppt keep alive, mppt is in normal mode
void Communication_ReceivedMPPTKeepAliveDisable()
{
  Power_Control_TurnOffMPPTKeepAlive();
}
