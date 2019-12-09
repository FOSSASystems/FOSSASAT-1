/*
 * FOSSA Ground Station Example
 *
 * Tested on Arduino Uno and SX1268, can be used with any LoRa radio
 * from the SX126x series. Make sure radio type (line 37)
 * and pin mapping (lines 23 - 25) match your hardware!
 *
 * References:
 *
 * RadioLib error codes:
 * https://jgromes.github.io/RadioLib/group__status__codes.html
 *
 * FOSSASAT-1 Communication Guide:
 * https://github.com/FOSSASystems/FOSSASAT-1/blob/master/FOSSA%20Documents/FOSSASAT-1%20Comms%20Guide.pdf
 *
 */

// include all libraries
#include <RadioLib.h>
#include <FOSSA-Comms.h>

// pin definitions
#define CS                10
#define DIO1              2
#define BUSY              9

// modem configuration
#define FREQUENCY         436.7   // MHz
#define BANDWIDTH         125.0   // kHz
#define SPREADING_FACTOR  11
#define CODING_RATE       8       // 4/8
#define SYNC_WORD         0x0F0F  // sync word
#define OUTPUT_POWER      21      // dBm
#define CURRENT_LIMIT     200     // mA

// set up radio module
SX1268 radio = new Module(CS, DIO1, BUSY);

// flags
volatile bool interruptEnabled = true;
volatile bool transmissionReceived = false;

// satellite callsign
char callsign[] = "FOSSASAT-1";

// radio ISR
void onInterrupt() {
  if(!interruptEnabled) {
    return;
  }

  transmissionReceived = true;
}

// function to print controls
void printControls() {
  Serial.println(F("------------- Controls -------------"));
  Serial.println(F("p - send ping frame"));
  Serial.println(F("i - request satellite info"));
  Serial.println(F("l - request last packet info"));
  Serial.println(F("r - send message to be retransmitted"));
  Serial.println(F("------------------------------------"));
}

void sendPing() {
  Serial.print(F("Sending ping frame ... "));

  // data to transmit
  uint8_t functionId = CMD_PING;

  // build frame
  uint8_t len = FCP_Get_Frame_Length(callsign);
  uint8_t* frame = new uint8_t[len];
  FCP_Encode(frame, callsign, functionId);

  // send data
  int state = radio.transmit(frame, len);
  delete[] frame;

  // check transmission success
  if (state == ERR_NONE) {
    Serial.println(F("sent successfully!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}

void requestInfo() {
  Serial.print(F("Requesting system info ... "));

  // data to transmit
  uint8_t functionId = CMD_TRANSMIT_SYSTEM_INFO;

  // build frame
  uint8_t len = FCP_Get_Frame_Length(callsign);
  uint8_t* frame = new uint8_t[len];
  FCP_Encode(frame, callsign, functionId);

  // send data
  int state = radio.transmit(frame, len);
  delete[] frame;

  // check transmission success
  if (state == ERR_NONE) {
    Serial.println(F("sent successfully!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}

void requestPacketInfo() {
  Serial.print(F("Requesting last packet info ... "));

  // data to transmit
  uint8_t functionId = CMD_GET_LAST_PACKET_INFO;

  // build frame
  uint8_t len = FCP_Get_Frame_Length(callsign);
  uint8_t* frame = new uint8_t[len];
  FCP_Encode(frame, callsign, functionId);

  // send data
  int state = radio.transmit(frame, len);
  delete[] frame;

  // check transmission success
  if (state == ERR_NONE) {
    Serial.println(F("sent successfully!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}

void requestRetransmit() {
  Serial.println(F("Enter message to be sent:"));
  Serial.println(F("(max 32 characters, end with LF or CR+LF)"));

  // get data to be retransmited
  char optData[32];
  uint8_t bufferPos = 0;
  while(bufferPos < 32) {
    while(!Serial.available());
    char c = Serial.read();
    Serial.print(c);
    if((c != '\r') && (c != '\n')) {
      optData[bufferPos] = c;
      bufferPos++;
    } else {
      break;
    }
  }

  // wait for a bit to receive any trailing characters
  delay(100);

  // dump the serial buffer
  while(Serial.available()) {
    Serial.read();
  }

  Serial.println();
  Serial.print(F("Requesting retransmission ... "));

  // data to transmit
  uint8_t functionId = CMD_RETRANSMIT;
  optData[bufferPos] = '\0';
  uint8_t optDataLen = strlen(optData);

  // build frame
  uint8_t len = FCP_Get_Frame_Length(callsign, optDataLen);
  uint8_t* frame = new uint8_t[len];
  FCP_Encode(frame, callsign, functionId, optDataLen, (uint8_t*)optData);

  // send data
  int state = radio.transmit(frame, len);
  delete[] frame;

  // check transmission success
  if (state == ERR_NONE) {
    Serial.println(F("sent successfully!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("FOSSA Ground Station Demo Code"));

  // initialize the radio
  int state = radio.begin(FREQUENCY,
                          BANDWIDTH,
                          SPREADING_FACTOR,
                          CODING_RATE,
                          SYNC_WORD,
                          OUTPUT_POWER,
                          CURRENT_LIMIT);

  if(state == ERR_NONE) {
    Serial.println(F("Radio initialization successful!"));
  } else {
    Serial.print(F("Failed to initialize radio, code: "));
    Serial.println(state);
    while(true);
  }

  // attach the ISR to radio interrupt
  radio.setDio1Action(onInterrupt);

  // begin listening for packets
  radio.startReceive();

  printControls();
}

void loop() {
  // check serial data
  if(Serial.available()) {
    // disable reception interrupt
    interruptEnabled = false;
    detachInterrupt(digitalPinToInterrupt(DIO1));

    // get the first character
    char serialCmd = Serial.read();

    // wait for a bit to receive any trailing characters
    delay(50);

    // dump the serial buffer
    while(Serial.available()) {
      Serial.read();
    }

    // process serial command
    switch(serialCmd) {
      case 'p':
        sendPing();
        break;
      case 'i':
        requestInfo();
        break;
      case 'l':
        requestPacketInfo();
        break;
      case 'r':
        requestRetransmit();
        break;
      default:
        Serial.print(F("Unknown command: "));
        Serial.println(serialCmd);
        break;
    }

    // set radio mode to reception
    radio.setDio1Action(onInterrupt);
    radio.startReceive();
    interruptEnabled = true;
  }

  // check if new data were received
  if(transmissionReceived) {
    // disable reception interrupt
    interruptEnabled = false;
    transmissionReceived = false;

    // read received data
    size_t respLen = radio.getPacketLength();
    uint8_t* respFrame = new uint8_t[respLen];
    int state = radio.readData(respFrame, respLen);

    // check reception success
    if (state == ERR_NONE) {
      // print raw data
      Serial.print(F("Received "));
      Serial.print(respLen);
      Serial.println(F(" bytes:"));
      PRINT_BUFF(respFrame, respLen);

      // get function ID
      uint8_t functionId = FCP_Get_FunctionID(callsign, respFrame, respLen);
      Serial.print(F("Function ID: 0x"));
      Serial.println(functionId, HEX);

      // check optional data
      uint8_t* respOptData = nullptr;
      uint8_t respOptDataLen = FCP_Get_OptData_Length(callsign, respFrame, respLen);
      Serial.print(F("Optional data ("));
      Serial.print(respOptDataLen);
      Serial.println(F(" bytes):"));
      if(respOptDataLen > 0) {
        // read optional data
        respOptData = new uint8_t[respOptDataLen];
        FCP_Get_OptData(callsign, respFrame, respLen, respOptData);
        PRINT_BUFF(respFrame, respLen);
      }

      // process received frame
      switch(functionId) {
        case RESP_PONG:
          Serial.println(F("Pong!"));
          break;

        case RESP_SYSTEM_INFO:
          Serial.println(F("System info:"));

          Serial.print(F("batteryChargingVoltage = "));
          Serial.println(FCP_Get_Battery_Charging_Voltage(respOptData));

          Serial.print(F("batteryChargingCurrent = "));
          Serial.println(FCP_Get_Battery_Charging_Current(respOptData), 4);

          Serial.print(F("batteryVoltage = "));
          Serial.println(FCP_Get_Battery_Voltage(respOptData));

          Serial.print(F("solarCellAVoltage = "));
          Serial.println(FCP_Get_Solar_Cell_Voltage(0, respOptData));

          Serial.print(F("solarCellBVoltage = "));
          Serial.println(FCP_Get_Solar_Cell_Voltage(1, respOptData));

          Serial.print(F("solarCellCVoltage = "));
          Serial.println(FCP_Get_Solar_Cell_Voltage(2, respOptData));

          Serial.print(F("batteryTemperature = "));
          Serial.println(FCP_Get_Battery_Temperature(respOptData));

          Serial.print(F("boardTemperature = "));
          Serial.println(FCP_Get_Board_Temperature(respOptData));

          Serial.print(F("mcuTemperature = "));
          Serial.println(FCP_Get_MCU_Temperature(respOptData));

          Serial.print(F("resetCounter = "));
          Serial.println(FCP_Get_Reset_Counter(respOptData));

          Serial.print(F("powerConfig = 0b"));
          Serial.println(FCP_Get_Power_Configuration(respOptData), BIN);
          break;

        case RESP_LAST_PACKET_INFO:
          Serial.println(F("Last packet info:"));

          Serial.print(F("SNR = "));
          Serial.print(respOptData[0] / 4.0);
          Serial.println(F(" dB"));

          Serial.print(F("RSSI = "));
          Serial.print(respOptData[1] / -2.0);
          Serial.println(F(" dBm"));
          break;

        case RESP_REPEATED_MESSAGE:
          Serial.println(F("Got repeated message:"));
          for(uint8_t i = 0; i < respOptDataLen; i++) {
            Serial.write(respOptData[i]);
          }
          Serial.println();
          break;

        default:
          Serial.println(F("Unknown function ID!"));
          break;
      }

      printControls();
      if(respOptDataLen > 0) {
        delete[] respOptData;
      }

    } else {
      Serial.print(F("Reception failed, code "));
      Serial.println(state);

    }

    // enable reception interrupt
    delete[] respFrame;
    radio.startReceive();
    interruptEnabled = true;
  }
}
