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

/**
 * This function takes a transmission frame and handles its data.
 */
void ProcessTransmission(size_t* responsePacketLength, uint8_t* responsePacketFrame)
{
  // get the function id
  uint8_t functionId = FCP_Get_FunctionID(callsign, responsePacketFrame, responsePacketLength);

  // get the packets length.
  uint8_t responsePacketDataLength = FCP_Get_OptData_Length(callsign, responsePacketFrame, responsePacketLength);


  // these variables store the optional data the packet has.
  uint8_t* responsePacketData = null;
  bool hasData = false;
  
  if (responsePacketDataLength > 0) // if we have data in packet.
  { 
    // extract the data to be used in the switch case later.
    responsePacketData = new uint8_t[responsePacketDataLength];
    
    hasData = true;
  } 
  
  
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

  // make sure we free any memory allocated for the optional data.
  if (hasData)
  {
    delete responsePacketData;
  }
}

void loop()
{
  // if we receive a transmission.
  if (isTransmissionReceived)
  {
    // stop transmissions from being received.
    isInterruptEnabled = false;

    // get frame info and data.
    size_t responsePacketLength = radio.getPacketLength();
    uint8_t* reponsePacketFrame = new uint8_t[responsePacketLength];

    // process the frame.
    int state = radio.readData(responsePacketFrame, responsePacketLength);
    if (state != ERR_NONE)
    {
      // error
      Serial.println(state);
    }
    else
    {
      ProcessTransmission(reponsePacketLength, responsePacketFrame);
    }


    // free the memory.
    delete responsePacketFrame;

    // enable receiving transmissions again.
    isInterruptEnabled = true;
  }
}
