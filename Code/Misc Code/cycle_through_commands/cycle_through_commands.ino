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

// Non ISM band LORA mode
#define CARRIER_FREQUENCY 434.0f // MHz
#define BANDWIDTH 125.0f //62.5f // KHz low bandiwdth mode for communicating with ground stations
#define SPREADING_FACTOR 9
#define CODING_RATE 8 // can be 12.
#define OUTPUT_POWER 6 // dBm

// Non ISM band FSK mode.
#define FSK_CARRIER_FREQUENCY 434.0f
#define FSK_CURRENT_LIMIT 10
#define FSK_FREQUENCY_DERIVATION 10.0
#define FSK_RX_BANDWIDTH 250.0
#define FSK_BIT_RATE 100.0
#define FSK_DATA_SHAPING 0.5
#define FSK_OUTPUT_POWER 10 // dBm

// ISM band LORA mode.
#define ISM_CARRIER_FREQUENCY 434.0f
#define ISM_BANDWIDTH 125.0f 
#define ISM_SPREADING_FACTOR 9
#define ISM_CODING_RATE 8 // can be 12.
#define ISM_OUTPUT_POWER 10 // dBm

// ISM band FSK mode
#define ISM_FSK_CARRIER_FREQUENCY 434.0f
#define ISM_FSK_CURRENT_LIMIT 10
#define ISM_FSK_FREQUENCY_DERIVATION 10.0
#define ISM_FSK_RX_BANDWIDTH 250.0
#define ISM_FSK_BIT_RATE 100.0
#define ISM_FSK_DATA_SHAPING 0.5
#define ISM_FSK_OUTPUT_POWER 10 // dBm

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

/*
 * @brief switches SX1278 modem mode to FSK.
 */
void Communication_SwitchFSK_ISM()
{
  int err, startupErr;
  
  startupErr = LORA.beginFSK();
  
  err = LORA.setFrequency(ISM_FSK_CARRIER_FREQUENCY);
  
  err = LORA.setBitRate(ISM_FSK_BIT_RATE);
  
  err = LORA.setFrequencyDeviation(ISM_FSK_FREQUENCY_DERIVATION);
  
  err = LORA.setRxBandwidth(ISM_FSK_RX_BANDWIDTH);
  
  err = LORA.setOutputPower(ISM_FSK_OUTPUT_POWER);
  
  err = LORA.setCurrentLimit(ISM_FSK_CURRENT_LIMIT);
  
  err = LORA.setDataShaping(ISM_FSK_DATA_SHAPING);
  
  uint8_t syncWord[] = { SYNC_WORD };
  err = LORA.setSyncWord(syncWord, 1);
}

void Communication_SwitchLORA()
{
  int err;
  err = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);
}

void Communication_SwitchLORA_ISM()
{
  int err;
  err = LORA.begin(ISM_CARRIER_FREQUENCY, ISM_BANDWIDTH, ISM_SPREADING_FACTOR, ISM_CODING_RATE, SYNC_WORD, ISM_OUTPUT_POWER);
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
}

String WaitForPacket()
{
  String str = "RX_TIMEOUT";
  int state;
  state = LORA.receive(str);

  unsigned long beginWaitTimestamp = millis();
  
  while (state != ERR_NONE)
  {
    unsigned long timeWaited = millis() - beginWaitTimestamp;
    
    if (timeWaited > 15000)
    {
      break;
    }
    
    delay(10);
  }

  return str;
}

void loop()
{
  Communication_SwitchLORA();

  // wait for a packet from the satellite.
  String rx_msg;
  int state;
  
  rx_msg = WaitForPacket();
  Serial.print("1) Start of the loop RX: ");
  Serial.println(rx_msg);
  
  // transmit a ping
  state = LORA.transmit("FOSSASAT-15;");
  if (state == ERR_NONE)
  {
    Serial.println("1) Sent PING command");
  }

 ;
  rx_msg = WaitForPacket();
  Serial.print("PING RX: ");
  Serial.println(rx_msg);
  
  
  // change the callsign
  state = LORA.transmit("FOSSASAT-113;1-TASASSOF");
  if (state == ERR_NONE)
  {
    Serial.println("2) Sent callsign change command");
  }
  
 ;
  rx_msg = WaitForPacket();
  Serial.print("CALLSIGN RX: ");
  Serial.println(rx_msg);


  // stop transmissions
  state = LORA.transmit("FOSSASAT-17;");
  if (state == ERR_NONE)
  {
    Serial.println("3) Sent transmission stop message");
  }
  ;
 
  rx_msg = WaitForPacket();
  Serial.print("Stop transmissions RX: ");
  Serial.println(rx_msg);

  // start transmissions
  state = LORA.transmit("FOSSASAT-18;");
  if (state == ERR_NONE)
  {
    Serial.println("4) Start transmissions again.");
  }

  ;
  rx_msg = WaitForPacket();
  Serial.print("Start transmissions RX: ");
  Serial.println(rx_msg);


    // repeater message transmissions
  state = LORA.transmit("FOSSASAT-115;repeater test");
  if (state == ERR_NONE)
  {
    Serial.println("5) Sent repeater test message.");
  }

  ;
  rx_msg = WaitForPacket();
  Serial.print("repeater message RX: ");
  Serial.println(rx_msg);

  

/*      // reset the eeprom
  state = LORA.transmit("FOSSASAT-110;");
  if (state == ERR_NONE)
  {
    Serial.println("Sent a reset the eeprom message.");
  }

  ;
  rx_msg = WaitForPacket();
  Serial.print("eeprom wipe message RX: ");
  Serial.println(rx_msg);*/
  

        // deploy
  state = LORA.transmit("FOSSASAT-111;");
  if (state == ERR_NONE)
  {
    Serial.println("6) Sent a deploy");
  }

  ;
  rx_msg = WaitForPacket();
  Serial.print("deploy message RX: ");
  Serial.println(rx_msg);


          // restart the satellite
  state = LORA.transmit("FOSSASAT-114;");
  if (state == ERR_NONE)
  {
    Serial.println("7) Sent a restart command");
  }

  ;
  rx_msg = WaitForPacket();
  Serial.print("restart command RX: ");
  Serial.println(rx_msg);





          // lock the satellite
  state = LORA.transmit("FOSSASAT-17;");
  if (state == ERR_NONE)
  {
    Serial.println("8) Sent a stop transmitting command");
  }

  ;
  rx_msg = WaitForPacket();
  Serial.print("stop transmitting RX: ");
  Serial.println(rx_msg);

  
            // restart the satellite
  state = LORA.transmit("FOSSASAT-114;");
  if (state == ERR_NONE)
  {
    Serial.println("9) Sent a restart command");
  }

  
  rx_msg = WaitForPacket();
  Serial.print("restart command RX: ");
  Serial.println(rx_msg);

          // unlock the satellite
  state = LORA.transmit("FOSSASAT-18;");
  if (state == ERR_NONE)
  {
    Serial.println("10) Sent a start transmitting command");
  }
  
  rx_msg = WaitForPacket();
  Serial.print("start transmitting RX: ");
  Serial.println(rx_msg);
  
  


  LORA.standby();
  delay(250);
}
