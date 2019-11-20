/*
 * FOSSASAT-1 Simulator
 *
 * The purpose of this code is to simulate FOSSASAT-1 communication, to verify
 * ground station setup. Please note that not all functionality of FOSSASAT-1
 * is implemented in the simulator, only the following:
 *  - ping/pong exchange
 *  - message retransmission
 *  - requesting system info and last packet info
 *  - modem switching
 *  - automated system info transmissions, both LoRa and RTTY
 *
 * Tested on Arduino Uno and SX1268, can be used with any LoRa radio
 * from the SX127x or SX126x series. Radio module type can be changed
 * in configuration.h, line 49 by changing the value of RADIO_TYPE.
 * Also, make sure all pins in configuration.h (line 33 - 40) match you hardware!
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

 #include "Simulator.h"

void setup() {
  Serial.begin(9600);
  Serial.println(F("FOSSASAT-1 Simulator"));

  Configuration_Setup_Pins();

  Persistent_Storage_Wipe();

  // enable interrupts and set default modem
  Communication_Set_Modem(currentModem);

  // set receive ISR
  #ifdef RADIO_SX126X
  radio.setDio1Action(Communication_Receive_Interrupt);
  #else
  radio.setDio0Action(Communication_Receive_Interrupt);
  #endif
  radio.startReceive();

  // start modem switch timer
  Timer1.initialize(MODEM_SWITCHING_PERIOD_FSK);
  Timer1.attachInterrupt(Communication_Change_Modem);
}

void loop() {
  // check received data flag
  if(dataReceived) {
    // disable interrupts
    interruptsEnabled = false;

    // read data
    size_t len = radio.getPacketLength();
    uint8_t* frame = new uint8_t[len];
    int16_t state = radio.readData(frame, len);

    // check reception state
    if(state == ERR_NONE) {
      FOSSASAT_DEBUG_PRINT(F("Got frame "));
      FOSSASAT_DEBUG_PRINTLN(len);
      FOSSASAT_DEBUG_PRINT_BUFF(frame, len);

      // check callsign
      uint8_t callsignLen = Persistent_Storage_Read<uint8_t>(EEPROM_CALLSIGN_LEN_ADDR);
      char* callsign = new char[callsignLen];
      System_Info_Get_Callsign(callsign, callsignLen);
      if(memcmp(frame, (uint8_t*)callsign, callsignLen - 1) == 0) {
        // check passed
        Comunication_Parse_Frame(frame, len);
      } else {
        FOSSASAT_DEBUG_PRINTLN(F("Callsign mismatch!"));
      }
      delete[] callsign;

    }

    // deallocate memory
    delete[] frame;

    // reset flag
    dataReceived = false;

    // enable interrupts
    interruptsEnabled = true;
  }

  // check modem switch
  if(switchModem) {
    // disable interrupts
    interruptsEnabled = false;

    // update modem
    switch(currentModem) {
      case MODEM_FSK_NON_ISM:
        currentModem = MODEM_LORA_ISM;
        Timer1.setPeriod(MODEM_SWITCHING_PERIOD_LORA_ISM);
        break;
      case MODEM_LORA_ISM:
        currentModem = MODEM_LORA_NON_ISM;
        Timer1.setPeriod(MODEM_SWITCHING_PERIOD_LORA_NON_ISM);
        break;
      case MODEM_LORA_NON_ISM:
        currentModem = MODEM_FSK_NON_ISM;
        Timer1.setPeriod(MODEM_SWITCHING_PERIOD_FSK);
        break;
    }
    Communication_Set_Modem(currentModem);

    // restart listen mode
    radio.startReceive();

    // reset flag
    switchModem = false;

    // enable interrupts
    interruptsEnabled = true;
  }

  // check elapsed time since last system info transmissions
  if(millis() - lastTransmit >= SYSTEM_INFO_TRANSMIT_PERIOD) {
    // disable interrupts
    interruptsEnabled = false;

    // send system info
    Communication_Send_System_Info();

    // update timestamp
    lastTransmit = millis();

    // enable interrupts
    interruptsEnabled = true;
  }

  // transmit system info frame via RTTY
  if(millis() - lastRtty >= RTTY_SYSTEM_INFO_PERIOD) {
    // disable interrupts
    interruptsEnabled = false;

    // send system info
    Communication_Send_System_Info(true);

    // save timestamp
    lastRtty = millis();

    // enable interrupts
    interruptsEnabled = true;
  }
}
