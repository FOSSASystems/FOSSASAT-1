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

#define CS 10
#define DIO0 2
#define DIO1 6

SX1278 radio = new Module(CS, DIO0, DIO1);

volatile bool isInterruptEnabled = true;
volatile bool isTransmissionReceived = false;

char callsign[] = "FOSSASAT-1";

/*
 * Utility functions
 */

void onInterrupt()
{
  if (!isInterruptEnabled) return;
  
  isTransmissionReceived = true;
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("FOSSA Ground Station Initializing..."));

  // initialize the radio.
  int state = radio.begin(434.0, 125.0, 11, 8, 0x0F0F);
  if (state == ERR_NONE)
  {
    Serial.println(F("Successfully initialized radio module!"));
  }
  else
  {
    Serial.print(F("Failed to initialize radio modue error code: "));
    Serial.println(state);
  }

  // attach the ISR to this function
  radio.setDio1Action(onInterrupt);
  // begin listening for packets.
  radio.startReceive();  

  Serial.println(F("Completed."));
}

/**
 * This function takes a transmission frame and handles its data.
 */
void ProcessReceivedTransmission(size_t* responsePacketLength, uint8_t* responsePacketFrame)
{
  // get the function id
  uint8_t functionId = FCP_Get_FunctionID(callsign, responsePacketFrame, responsePacketLength);

  // get the packets length.
  uint8_t responsePacketDataLength = FCP_Get_OptData_Length(callsign, responsePacketFrame, responsePacketLength);


  // these variables store the optional data the packet has.
  uint8_t* responsePacketData;
  bool hasData = false;
  
  if (responsePacketDataLength > 0) // if we have data in packet.
  { 
    // extract the data to be used in the switch case later.
    responsePacketData = new uint8_t[responsePacketDataLength];
    
    hasData = true;
  } 

  Serial.print(F("Received: "));
  
  switch (functionId)
  {
    case RESP_PONG:
      Serial.println(F(" PONG"));
      break;
    case RESP_REPEATED_MESSAGE:
      PRINT_BUFF(responsePacketData, responsePacketDataLength);
      break;
    case RESP_REPEATED_MESSAGE_CUSTOM:
      PRINT_BUFF(responsePacketData, responsePacketDataLength);
      break;
    case RESP_SYSTEM_INFO:
      PRINT_BUFF(responsePacketData, responsePacketDataLength);
      break;
    case RESP_LAST_PACKET_INFO:
      PRINT_BUFF(responsePacketData, responsePacketDataLength);
      break;
  }

  // make sure we free any memory allocated for the optional data.
  if (hasData)
  {
    delete responsePacketData;
  }
}

void SendTransmission(int serialDataLength)
{
  // extract the message from the serial input.
  char* message = new char[serialDataLength];
  
  for (int i = 0; i < serialDataLength; i++)
  {
    message[i] = (char)(Serial.read());
  }

  Serial.print(F("Transmitting: "));
  Serial.println(message);


  // get the information for the frame.
  char* data;
  uint8_t functionId;
  uint8_t dataLength;
  bool includesData;

  if (strcmp(message, "PING"))
  {
    functionId = CMD_PING;
    includesData = false;
  }
  else if (strcmp(message, "GET_SYSTEM_INFO"))
  {
    functionId = CMD_TRANSMIT_SYSTEM_INFO;
    includesData = false;
  }
  else if (strcmp(message, "GET_LAST_PACKET_INFO"))
  {
    functionId = CMD_GET_LAST_PACKET_INFO;
    includesData = false;
  }
  else
  {
    functionId = CMD_RETRANSMIT;
    includesData = true;
    dataLength = strlen(message);
    data = message;    
  }


  // construct the frame.
  uint8_t frameLength;
  if (includesData)
  {
    frameLength = FCP_Get_Frame_Length(callsign, dataLength, (uint8_t*)data);
  }
  else
  {
    frameLength = FCP_Get_Frame_Length(callsign);
  }
  
  uint8_t* frame = new uint8_t[frameLength];
  if (includesData)
  {
    FCP_Encode(frame, callsign, functionId, data, (uint8_t)data); 
  }
  else
  {
    FCP_Encode(frame, callsign, functionId);
  }


  // transmit the frame.
  int state = radio.transmit(frame, frameLength);
  if (state == ERR_NONE)
  {
    Serial.println(F("Successfully transmitted!"));
  }
  else
  {
    Serial.println(F("Error transmitting message..."));
  }

  delete[] frame;
  delete[] message;
}

void loop()
{

  
  // if we receive a serial message.
  if (Serial.available() > 0)
  {
    // prevent receive interrupt.
    isInterruptEnabled = false;
    detachInterrupt(digitalPinToInterrupt(DIO1));

    // send a command.
    SendTransmission(Serial.available());

    // re-enable radio into receive mode.
    radio.setDio1Action(onInterrupt);
    isInterruptEnabled = true;
  }


  
  
  // if we receive a transmission.
  if (isTransmissionReceived)
  {
    // stop transmissions from being received.
    isInterruptEnabled = false;

    // get frame info and data.
    size_t responsePacketLength = radio.getPacketLength();
    uint8_t* responsePacketFrame = new uint8_t[responsePacketLength];

    // process the frame.
    int state = radio.readData(responsePacketFrame, responsePacketLength);
    if (state != ERR_NONE)
    {
      // error
      Serial.println(state);
    }
    else
    {
      ProcessReceivedTransmission(responsePacketLength, responsePacketFrame);
    }


    // free the memory.
    delete[] responsePacketFrame;

    // enable receiving transmissions again.
    isInterruptEnabled = true;
  }


  
}
