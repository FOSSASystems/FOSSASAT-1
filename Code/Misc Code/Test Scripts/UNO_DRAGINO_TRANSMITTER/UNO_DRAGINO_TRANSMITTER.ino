#include <Arduino.h>
#include <RadioLib.h>


#define ANTENNA_NSS 10
#define ANTENNA_DIO0 2
#define ANTENNA_DIO1 6

SX1278 LORA = new Module(ANTENNA_NSS, ANTENNA_DIO0, ANTENNA_DIO1);//new Module(7, 2, 3);

/////////////////////////////////////
// Radio (Lora chip) configuration //
/////////////////////////////////////
// All configurations.
#define SYNC_WORD 0x0F

#define LOWER_SPREADING_FACTOR 7

// Non ISM band LORA mode
#define CARRIER_FREQUENCY 434.0f // MHz
#define BANDWIDTH 125.0f //62.5f // KHz low bandiwdth mode for communicating with ground stations
#define SPREADING_FACTOR 11
#define ALTERNATIVE_SPREADING_FACTOR 10
#define CODING_RATE 8 // can be 12.
#define OUTPUT_POWER 6 // dBm

// ISM band LORA mode.
#define ISM_CARRIER_FREQUENCY 433.0f
#define ISM_BANDWIDTH 125.0f 
#define ISM_SPREADING_FACTOR 11
#define ISM_ALTERNATIVE_SPREADING_FACTOR 10
#define ISM_CODING_RATE 8 // can be 12.
#define ISM_OUTPUT_POWER 10 // dBm

// Non ISM band FSK mode.
#define FSK_CARRIER_FREQUENCY 434.0f
#define FSK_CURRENT_LIMIT 10
#define FSK_FREQUENCY_DERIVATION 10.0
#define FSK_RX_BANDWIDTH 250.0
#define FSK_BIT_RATE 100.0
#define FSK_DATA_SHAPING 0.5
#define FSK_OUTPUT_POWER 10 // dBm


void Communication_SwitchFSK()
{
  int err, startupErr;
  
  startupErr = LORA.beginFSK();
  err = LORA.setFrequency(FSK_CARRIER_FREQUENCY);  
  err = LORA.setBitRate(FSK_BIT_RATE);
  
  err = LORA.setFrequencyDeviation(FSK_FREQUENCY_DERIVATION);
    
  err = LORA.setRxBandwidth(FSK_RX_BANDWIDTH);
  
  err = LORA.setOutputPower(FSK_OUTPUT_POWER);
  
  err = LORA.setCurrentLimit(FSK_CURRENT_LIMIT);
  
  err = LORA.setDataShaping(FSK_DATA_SHAPING);
  
  uint8_t syncWord[] = { SYNC_WORD };
  err = LORA.setSyncWord(syncWord, 1);
}


void Communication_SwitchLORA()
{
  int err;
  err = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);
    Serial.println(err);
}

void Communication_SwitchLORA_ISM()
{
  int err;
  err = LORA.begin(ISM_CARRIER_FREQUENCY, ISM_BANDWIDTH, ISM_SPREADING_FACTOR, ISM_CODING_RATE, SYNC_WORD, ISM_OUTPUT_POWER);
  Serial.println(err);
}


/*
 * 1) Start Transmitting
 * 2) Set callsign to "FOSSASAT-1"
 * 3) Repeat message "RY"
 * 4) Send PING transmission.
 * 5) Stop Transmitting.
 */
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Started test ground station");
  Communication_SwitchLORA();

}
void loop()
{
//  // wait for a packet from the satellite.
  String str;
  int state;
//
 state = LORA.transmit("FOSSASAT-120;");
 
  if (state == ERR_NONE) {
  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(" too long!");

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(" timeout!");

  }
//
//
// state = LORA.receive(str);
//  
// if (state == ERR_NONE) {
//    // print the data of the packet
//    Serial.print("[SX1278] Data:\t\t");
//    Serial.println(str);
//
//  } else if (state == ERR_RX_TIMEOUT) {
//  } else if (state == ERR_CRC_MISMATCH) {
//    // packet was received, but is malformed
//    Serial.println(F("CRC error!"));
//
//  }
  
  /*
          // unlock the satellite
  state = LORA.transmit("FOSSASAT-18;");
  if (state == ERR_NONE)
  {
    Serial.println("10) Sent a start transmitting command");
  }
  */
  
}
