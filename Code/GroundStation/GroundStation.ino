/*
 * FOSSA Ground Station Example
 * 
 * Setup is an arduino uno with a sx1278.
 * 
 * To use this program, plug it into your computer and send
 * commands via the arduino serial monitor.
 * 
 * Richard B.
 * 
 * 
 * References:
 * 
 * radio.readData(...) Error code values - https://github.com/jgromes/RadioLib/blob/master/src/TypeDef.h
 * 
 */

#include <RadioLib.h>
#include <FOSSA-Comms.h>

SX1278 radio = new Module(10, 2, 6);

volatile bool isInterruptEnabled = true;
volatile bool isTransmissionReceived = false;

char callsign[] = "FOSSASAT-1";

/*
 * Utility functions
 */

 /*
  * This function returns true if the radio was set up correctly.
  */
bool CheckRadioStatusCode(int pErrorCode)
{
  
}

void onInterrupt()
{
  if (!isInterruptEnabled) return;
  
  transmissionRecieved = true;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("FOSSA Ground Station Initializing...");

  

  Serial.println("Completed.");
}

void loop()
{
  if (isTransmissionReceived)
  {
    isInterruptEnabled = false;
    
    size_t responsePacketLength = radio.getPacketLength();
    uint8_t* reponsePacketFrame = new uint8_t[responsePacketLength];

    int state = radio.readData(responsePacketFrame, responsePacketLength);

    switch (state)
    {
      case ERR_INVALID
    }
    if (state == ERR_NONE)
    {
      uint8_t functionId = FCP_Get_FunctionID(callsign, responsePacketFrame, responsePacketLength);

      switch functionId:
      {
        case RESP_PONG:
          break;
        case RESP_REPEATED_MESSAGE:
          break;
        case RESP_REPEATED_MESSAGE_CUSTOM:
          break;
        case RESP_SYSTEM_INFO:
          break;
        case RESP_LAST_PACKET_INFO:
          break;
      }
   /*   
      if (functionId == RESP_SYSTEM_INFO)
      {
        uint8_t responsePacketDataLength = FCP_Get_OptData_Length(callsign, responsePacketFrame, responsePacketLength);
  
        if (responsePacketDataLength > 0)
        { 
          uint8_t* responsePacketData = new uint8_t[responsePacketDataLength];
  
          delete responsePacketData;
        }        
      }
      */
    }

    delete responsePacketFrame;
    isInterruptEnabled = true;
  }
}
