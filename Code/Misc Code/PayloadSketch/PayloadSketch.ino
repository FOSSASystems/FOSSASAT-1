#include <Wire.h>

#define BEACON_WIRE_ADDR 1 // this is the address of the beacon when it joins the I2C bus.

bool TRANSMIT_MESSAGE_ENABLED = false; // this is switched when we have a message to send.
String TRANSMIT_MESSAGE = ""; // this is what is sent during the loop

void SendRadioMessage(String pMessage)
{
  TRANSMIT_MESSAGE_ENABLED = true;
  TRANSMIT_MESSAGE = pMessage;
}

void UpdateMessagePipeline()
{
  if (!TRANSMIT_MESSAGE_ENABLED) return; // do nothing if we have no message to send to the beacon
  
  Wire.beginTransmission(BEACON_WIRE_ADDR); // transmit the message
  Wire.println(TRANSMIT_MESSAGE);
  Wire.endTransmission();

  TRANSMIT_MESSAGE = "";
  TRANSMIT_MESSAGE_ENABLED = false; // prevent this code to be run again until the next message.
}

String BEACON_DATA = "";

void UpdateSatelliteInfo()
{
  BEACON_DATA = "";
  
  Wire.requestFrom(BEACON_WIRE_ADDR, 256);
  while (Wire.available())
  {
    char c = Wire.read();
    BEACON_DATA += c;
  }
}

void setup()
{
  Wire.begin(2);
  
  Serial.begin(9600);

  SendRadioMessage("RY");
}

void loop()
{
  UpdateMessagePipeline(); // send any messages we have to the beacon.
  UpdateSatelliteInfo(); // get the newest values from the beacon.

  // write your code here.
  
  delay(3000);
}
