// include the libraries
#include <FOSSA-Comms.h>
#include <RadioLib.h>

#define DEBUG_MODE true // disabling this macro means that debug information will print

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// DIO2 pin:  3
// BUSY pin:  9
SX1262 radio = new Module(10, 2, 3, 9);

// satellite callsign
char callsign[] = "FOSSASAT-1";

// last transmission timestamp
uint32_t lastTransmit = 0;

// transmission period in ms
const uint32_t transmitPeriod = 4000;

// interrupt flags
volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;

// interrupt service routine for data reception
void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }

  receivedFlag = true;
}

void setup() {
  Serial.begin(9600);

  // initialize SX1262
#ifdef DEBUG_MODE
  Serial.print(F("Initializing ... "));
#endif
 
  int state = radio.begin(436.7, 125.0, 11, 8, 0x0F0F);
  if (state == ERR_NONE) {
#ifdef DEBUG_MODE
    Serial.println(F("success!"));
#endif
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  /*
    // set TCXO reference voltage
    Serial.print(F("Setting TCXO reference ... "));
    state = radio.setTCXO(1.6);
    if (state == ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while (true);
    }
  */

  // set interrupt service routine
  radio.setDio1Action(setFlag);

  // start listening
  radio.startReceive();
}

void loop() {
  // check if it's time to transmit
  if(millis() - lastTransmit >= transmitPeriod) {
    // disable reception interrupt
    enableInterrupt = false;
    detachInterrupt(digitalPinToInterrupt(2));

    // save timestamp
    lastTransmit = millis();

#ifdef DEBUG_MODE
    Serial.println(F("Transmitting packet ... "));
#endif

    // data to transmit
    uint8_t functionId = CMD_TRANSMIT_SYSTEM_INFO;

    // build frame
    uint8_t len = FCP_Get_Frame_Length(callsign);
    uint8_t* frame = new uint8_t[len];
    FCP_Encode(frame, callsign, functionId);
    PRINT_BUFF(frame, len);

    // send data
    int state = radio.transmit(frame, len);
    delete[] frame;

    // check transmission success
    if (state == ERR_NONE) {
#ifdef DEBUG_MODE
      Serial.println(F("Success!"));
#endif
    }

    // set radio mode to reception
#ifdef DEBUG_MODE
    Serial.println(F("Waiting for response ... "));
#endif
    radio.setDio1Action(setFlag);
    radio.startReceive();
    enableInterrupt = true;
  }

  // check if new data were received
  if(receivedFlag) {
    // disable reception interrupt
    enableInterrupt = false;
    receivedFlag = false;

    // read received data
    size_t respLen = radio.getPacketLength();
    uint8_t* respFrame = new uint8_t[respLen];
    int state = radio.readData(respFrame, respLen);

    // check reception success
    if (state == ERR_NONE) {
      // print raw data
#ifdef DEBUG_MODE
      Serial.print(F("Received "));
      Serial.print(respLen);
      Serial.println(F(" bytes:"));
      PRINT_BUFF(respFrame, respLen);
#endif
      
      // get function ID
      uint8_t functionId = FCP_Get_FunctionID(callsign, respFrame, respLen);
#ifdef DEBUG_MODE
      Serial.print(F("Function ID: 0x"));
      Serial.println(functionId, HEX);
#endif

      // check optional data
      uint8_t respOptDataLen = FCP_Get_OptData_Length(callsign, respFrame, respLen);
      if(respOptDataLen > 0) {
        // frame contains optional data
        uint8_t* respOptData = new uint8_t[respOptDataLen];
        FCP_Get_OptData(callsign, respFrame, respLen, respOptData);

        // check system info response
        if(functionId == RESP_SYSTEM_INFO) {
#ifdef DEBUG_MODE
          Serial.println(F("System info:"));
          
          Serial.print(F("batteryChargingVoltage = "));
          Serial.println(FCP_Get_Battery_Charging_Voltage(respOptData));
          
          Serial.print(F("batteryChargingCurrent = "));
          Serial.println(FCP_Get_Battery_Charging_Current(respOptData));
          
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
          
          Serial.print(F("resetCounter = "));
          Serial.println(FCP_Get_Reset_Counter(respOptData));
          
          Serial.print(F("powerConfig = 0b"));
          Serial.println(FCP_Get_Power_Configuration(respOptData), BIN);
#endif
          Serial.print(FCP_Get_Battery_Temperature(respOptData));
          Serial.print(F("\t"));
          Serial.println(FCP_Get_Board_Temperature(respOptData));
          
        }
      }
      
    } else {
#ifdef DEBUG_MODE
      Serial.println(F("Reception failed, code "));
      Serial.println(state);
#endif
    }

    // enable reception interrupt
    delete[] respFrame;
    enableInterrupt = true;
  }

}
