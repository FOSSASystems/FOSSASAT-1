/*
 * @brief
 * 
 * @company FOSSA SYSTEMS
 * @authors Jan Gromes, Richard Bamford
 */

// include the libraries
#include <AESLib.h>
#include <RadioLib.h>

#define SD_LOGGING_ENABLED // to disable SD card logging, comment this line.

#define FOSSASAT_DEBUG_PRINT_BUFF(BUFF, LEN) { \
  for(size_t i = 0; i < LEN; i++) { \
    Serial.print(F("0x")); \
    Serial.print(BUFF[i], HEX); \
    Serial.print('\t'); \
    Serial.write(BUFF[i]); \
    Serial.println(); \
  } }


#ifdef SD_LOGGING_ENABLED
  #include <SPI.h>
  #include <SD.h>
  #include <EEPROM.h>
  
  /*
   * @brief This configures which cs pin maps to the SD card.
   */
  #define SD_CARD_CS 4
  
  /*
   * @breif This boolean signals whether the logging system can use the sd card.
   *        if the boolean is false (happens if SD.begin() fails) then the program will not
   *        use the sd card however it wont stop the program from executing.
    */
  bool SD_CARD_FUNCTIONAL = true;

  /*
  * @domain PRIVATE - DO NOT EDIT
  * @brief This variable is the container for the generated filename below - DO NOT EDIT
  *
  */
  String SD_CARD_FILENAME = "";

  /*
   * @domain PRIVATE
   * @brief this is the variable used when using SD.open();
   */
  File SD_CARD_OUTFILE;
 
  /*
   * @brief This function sets up the SD card's I2C bus.
   */
  void SDCard_Initialize()
  {
    if (!SD.begin(SD_CARD_CS))
    {
      Serial.println(F("Could not initialize SD card!"));
      SD_CARD_FUNCTIONAL = false;
      return;
    }

    // get the current stored EEPROM value for the recording.
    int ramRecordingIndex = 0;
    EEPROM.get(5, ramRecordingIndex);

    // increment it by 1.
    ramRecordingIndex++;
    EEPROM.put(5, ramRecordingIndex);

    SD_CARD_FILENAME = String(ramRecordingIndex) + String(".csv");

    Serial.print("Creating file called: ");
    Serial.println(SD_CARD_FILENAME);
    
    File newFile = SD.open(SD_CARD_FILENAME, O_RDWR | O_CREAT);
    if (!newFile)
    {
      Serial.print(F("Could not create file called: "));
      Serial.println(SD_CARD_FILENAME);
      SD_CARD_FUNCTIONAL = false;
      return;
    }
    else
    {
       Serial.print("File created called: ");
       Serial.println(SD_CARD_FILENAME);
		
    	newFile.close();
    }
  }
  
  /*
   * @brief This function takes the buffer received by the radio and appends it to a local file.
   */
  void SDCard_Log(uint8_t* pTransmissionBuffer, size_t pTransmissionBufferLength)
  {
    // if the sd card wasn't found, skip the interface.
    if (!SD_CARD_FUNCTIONAL)
    {
      Serial.println(F("[Failed logging to the SD card]"));
      return;
    }
    
    // create string bound for the SD card.
    String receivedRadioTransmission = "";
    
    // for each byte in the uint_8 array.
    // append the hex equivilent to the string.
    for (size_t i = 0; i < pTransmissionBufferLength; i++)
    {
      uint8_t transmissionByte = pTransmissionBuffer[i];
      String hexEquivilant = String(transmissionByte, HEX);
  
       receivedRadioTransmission += hexEquivilant;
    }
    receivedRadioTransmission += String("\n");


    // Try to append to the file.
    SD_CARD_OUTFILE = SD.open(SD_CARD_FILENAME, O_RDWR | O_APPEND);
    if (!SD_CARD_OUTFILE)
    {
      Serial.print(F("Could not open the file with name `"));
      Serial.print(SD_CARD_FILENAME);
      Serial.println(F("`"));
      
      SD_CARD_FUNCTIONAL = false;
      return;
    }

    // Write the transmission to it.
    Serial.println(F("[SD-CARD-LOGGER]"));
    Serial.println(receivedRadioTransmission);
    Serial.println(F("[END SD-CARD-LOGGER]"));
    
    SD_CARD_OUTFILE.println(receivedRadioTransmission);

    // Close the file (this also saves the file).
    SD_CARD_OUTFILE.close();
  }
#endif

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// DIO2 pin:  3
// BUSY pin:  9
SX1262 radio = new Module(10, 2, 3, 9);

char callsign[] = "FOSSASAT-1";

const char* password = "password";
const uint8_t encryptionKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;
uint32_t lastTransmit = 0;

void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }

  receivedFlag = true;
}

void setup() {
  Serial.begin(9600);

  // initialize with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x1424 (private network)
  // output power:                14 dBm
  // current limit:               60 mA
  // preamble length:             8 symbols
  // CRC:                         enabled
  int state = radio.begin(434.0, 125.0, 11, 8, 0x0F0F);

  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  /*
    Serial.print(F("Setting TCXO reference ... "));
    // enable TCXO
    // reference voltage:           1.6 V
    // timeout:                     5000 us
    state = radio.setTCXO(1.6);
    if (state == ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while (true);
    }
  */

  randomSeed(analogRead(A0)); 

  radio.setDio1Action(setFlag);

#ifdef SD_LOGGING_ENABLED
  SDCard_Initialize();
#endif
}

void loop() {
  if(millis() - lastTransmit >= 4000) {
    enableInterrupt = false;
    detachInterrupt(digitalPinToInterrupt(2));
    lastTransmit = millis();

    char optData[] = "FOSSASAT-1";
  
    // calculate padding length
    size_t optDataLen = strlen(optData) + strlen(password);
    size_t paddingLen = 16 - ((optDataLen + 1) % 16);
    size_t len = strlen(callsign) + 1 + 1 + optDataLen + paddingLen;
    uint8_t* frame = new uint8_t[len];
  
    // callsign
    memcpy(frame, callsign, strlen(callsign));
  
    // functionID
    frame[strlen(callsign)] = 0x24;
  
    // create buffer for encrypted section
    size_t encSectionLen = 1 + optDataLen + paddingLen;
    uint8_t* encSection = new uint8_t[encSectionLen];
  
    // optDataLen
    encSection[0] = optDataLen;
  
    // password
    memcpy(encSection + 1, password, strlen(password));
  
    // optData
    memcpy(encSection + 1 + strlen(password), optData, strlen(optData));
    
    // pad encrypted section up to mod 16
    for(size_t i = optDataLen + 1; i < encSectionLen; i++) {
      encSection[i] = (uint8_t)random(0x00, 0x100);
    }
  
    // encrypt the section
    aes128_enc_multiple(encryptionKey, encSection, encSectionLen);
  
    // append encrypted section to the frame
    memcpy(frame + strlen(callsign) + 1, encSection, encSectionLen);
    delete[] encSection;
  
    Serial.print(F("Transmitting packet ... "));
    int state = radio.transmit(frame, len);
    delete[] frame;

    if (state == ERR_NONE) {
      // the packet was successfully transmitted
      Serial.println(F("success!"));
    }

    Serial.println(F("Waiting for response ... "));
    radio.setDio1Action(setFlag);
    radio.startReceive();
    enableInterrupt = true;
  }
  
  if(receivedFlag) {
    enableInterrupt = false;
    receivedFlag = false;

    size_t respLen = radio.getPacketLength();
    uint8_t* respFrame = new uint8_t[respLen];
    int state = radio.readData(respFrame, respLen);
    if (state == ERR_NONE) {
      Serial.println(F("success!"));

      Serial.print(F("Received "));
      Serial.print(respLen);
      Serial.println(F(" bytes:"));
      FOSSASAT_DEBUG_PRINT_BUFF(respFrame, respLen);

#ifdef SD_LOGGING_ENABLED
      SDCard_Log(respFrame, respLen);
#endif
   
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);

    }
  } 
}
