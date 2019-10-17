/*
WORKING RECIEVER
*/
#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

// Using software SPI is really not suggested, its incredibly slow
//Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _mosi, _sclk, _rst, _miso);
// Use hardware SPI
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
#include <LoRaLib.h>
#include <SPI.h>
#include "LCD_Functions.h"
const int chipSelect = 4;

// NSS pin:   7
// DIO0 pin:  2
// DIO1 pin:  3
SX1278 lora = new LoRa;

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(ILI9340_WHITE);
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  int  state = lora.begin(434.0, 125, 11, 8, 0x13, 10);
  if (1 == 1) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);

  }
Serial.print("Initializing SD card...");

if (!SD.begin(chipSelect)) {

Serial.println("Card failed, or not present");

// don't do anything more:

return;

}

Serial.println("card initialized.");

}


void loop() {
  Serial.print("Waiting for incoming transmission ... ");

  // you can receive data as an Arduino String
  String str;
  int state = lora.receive(str);

   tft.fillRect(0, 85, 340, 350, ILI9340_WHITE);
 tft.setRotation(3);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_BLUE);  tft.setTextSize(3);
   tft.println("                 ");
  tft.println(" FOSSA SYSTEMS    RECIEVER");
  tft.setTextColor(ILI9340_BLACK);    tft.setTextSize(2); 
  tft.println(" ");
  tft.println(" DATA: ");
  tft.println(str);
  Serial.println(str);
  tft.print(" SNR:");
  tft.print(lora.lastPacketSNR);
  tft.print(" RSSI: ");
  tft.println(lora.lastPacketRSSI);
  tft.print(" ERROR: ");
  tft.print(lora.getFrequencyError());
 tft.print("Hz");
File sdcard_file = SD.open("data.txt", FILE_WRITE);

// if the file is available, write to it:

if (sdcard_file) {
sdcard_file.print("\n");
sdcard_file.print(" DATA:  ");

sdcard_file.print(str);

sdcard_file.print(" SNR:   ");

sdcard_file.print(lora.lastPacketSNR);
sdcard_file.print(" dBm ");

sdcard_file.print("  RSSI: ");

sdcard_file.print(lora.lastPacketRSSI);
sdcard_file.print(" dBm ");

sdcard_file.print(" Frequency Error: ");

sdcard_file.print(lora.getFrequencyError());

sdcard_file.print("  Hz ");



sdcard_file.close();
  if (state == ERR_NONE) {
    // packet was successfully received
    Serial.println("success!");

    // print data of the packet
    Serial.print("Data:\t\t\t");
    Serial.println(str);

    // print RSSI (Received Signal Strength Indicator) 
    // of the last received packet
    Serial.print("RSSI:\t\t\t");
    Serial.print(lora.lastPacketRSSI);
    Serial.println(" dBm");

    // print SNR (Signal-to-Noise Ratio) 
    // of the last received packet
    Serial.print("SNR:\t\t\t");
    Serial.print(lora.lastPacketSNR);
    Serial.println(" dBm");

    // print frequency error
    // of the last received packet
    Serial.print("Frequency error:\t");
    Serial.print(lora.getFrequencyError());
    Serial.println(" Hz");

  } else if (state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println("timeout!");
  }

   

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  
  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56,
                      0x78, 0xAB, 0xCD, 0xEF};
    int state = lora.transmit(byteArr, 8);
  */

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(" success!");

    // print measured data rate
    Serial.print("Datarate:\t");
    Serial.print(lora.dataRate);
    Serial.println(" bps");

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(" too long!");

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(" timeout!");
      delay(1000);

  }

}
}
