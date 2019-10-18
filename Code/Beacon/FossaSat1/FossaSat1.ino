#include "FossaSat1.h"

void setup() {
  // initialize debug port
  FOSSASAT_DEBUG_BEGIN(9600);

  // increment reset counter
  FOSSASAT_DEBUG_PRINT(F("Restart #"));
  FOSSASAT_DEBUG_PRINTLN(Persistent_Storage_Read<uint16_t>(EEPROM_RESTART_COUNTER_ADDR));
  Persistent_Storage_Write<uint16_t>(EEPROM_RESTART_COUNTER_ADDR, Persistent_Storage_Read<uint16_t>(EEPROM_RESTART_COUNTER_ADDR) + 1);

  // setup pins
  Configuration_Setup_Pins();

  #ifdef EEPROM_WIPE
    // wipe EEPROM
    Persistent_Storage_Wipe();
  #endif

  // check if this is the first run
  if(Persistent_Storage_Read<uint8_t>(EEPROM_FIRST_RUN_ADDR) != EEPROM_CONSECUTIVE_RUN) {
    // first run, set EEPROM flag
    Persistent_Storage_Write<uint8_t>(EEPROM_FIRST_RUN_ADDR, EEPROM_CONSECUTIVE_RUN);

  } else {
    // consecutive run, load power control configuration
    Power_Control_Load_Configuration();

  }

  // print power configuration
  FOSSASAT_DEBUG_PRINT(F("Config: 0b"));
  FOSSASAT_DEBUG_PRINTLN(powerConfig.val, BIN);

  // check deployment
  #ifdef ENABLE_DEPLOYMENT_SEQUENCE
    FOSSASAT_DEBUG_PRINT(F("Deploy #"));
    FOSSASAT_DEBUG_PRINTLN(Persistent_Storage_Read<uint8_t>(EEPROM_DEPLOYMENT_COUNTER_ADDR));
    // skip deployment for first restart (integration purposes) and try several deployment attempts
    if((Persistent_Storage_Read<uint8_t>(EEPROM_FIRST_RUN_ADDR) > (uint8_t)0) &&
       (Persistent_Storage_Read<uint8_t>(EEPROM_DEPLOYMENT_COUNTER_ADDR) < DEPLOYMENT_ATTEMPTS)) {

      // sleep before deployment
      Power_Control_Delay(DEPLOYMENT_SLEEP_LENGTH, true);

      // deploy
      Deployment_Deploy();
    }
  #endif

  // set temperature sensor resolution
  Pin_Interface_Set_Temp_Resolution(BOARD_TEMP_SENSOR_ADDR, TEMP_SENSOR_RESOLUTION_10_BITS);
  Pin_Interface_Set_Temp_Resolution(BATTERY_TEMP_SENSOR_ADDR, TEMP_SENSOR_RESOLUTION_10_BITS);

  // setup INA226
  Power_Control_Setup_INA226();

  // enable interrupts and set default modem
  Communication_Set_Modem(currentModem);

  // set receive ISR
  radio.setDio1Action(Communication_Receive_Interrupt);
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
        // check low power flag
        Power_Control_Load_Configuration();
        if(powerConfig.bits.lowPowerModeActive) {
          // skip ISM LoRa in low power mode
          currentModem = MODEM_LORA_NON_ISM;
          Timer1.setPeriod(MODEM_SWITCHING_PERIOD_LORA_NON_ISM);
        } else {
          currentModem = MODEM_LORA_ISM;
          Timer1.setPeriod(MODEM_SWITCHING_PERIOD_LORA_ISM);
        }
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

    // check low power flag
    #ifdef ENABLE_TRANSMISSION_CONTROL
      Power_Control_Load_Configuration();
      if((!powerConfig.bits.lowPowerModeActive) && (powerConfig.bits.transmitEnabled)) {
        Communication_Send_System_Info();
      } else {
        FOSSASAT_DEBUG_PRINTLN(F("Tx off"));
      }
    #else
      Communication_Send_System_Info();
    #endif

    // update timestamp
    lastTransmit = millis();

    // enable interrupts
    interruptsEnabled = true;
  }

  // transmit system info frame via RTTY
  if(millis() - lastRtty >= RTTY_SYSTEM_INFO_PERIOD) {
    // disable interrupts
    interruptsEnabled = false;

    // check low power flag
    #ifdef ENABLE_TRANSMISSION_CONTROL
      Power_Control_Load_Configuration();
      if((!powerConfig.bits.lowPowerModeActive) && (powerConfig.bits.transmitEnabled)) {
        Communication_Send_System_Info(true);
      } else {
        FOSSASAT_DEBUG_PRINTLN(F("Tx off"));
      }
    #else
      Communication_Send_System_Info(true);
    #endif

    // save timestamp
    lastRtty = millis();

    // enable interrupts
    interruptsEnabled = true;
  }

  // check battery voltage
  if(millis() - lastBatteryCheck >= BATTERY_CHECK_PERIOD) {
    FOSSASAT_DEBUG_PRINTLN(F("Battery check"));

    // load power configuration from EEPROM
    Power_Control_Load_Configuration();

    // check battery voltage
    if((Power_Control_Get_Battery_Voltage() <= BATTERY_VOLTAGE_LIMIT) && powerConfig.bits.lowPowerModeEnabled) {
      // activate low power mode
      powerConfig.bits.lowPowerModeActive = 1;
    } else {
      // deactivate low power mode
      powerConfig.bits.lowPowerModeActive = 0;
    }
    FOSSASAT_DEBUG_PRINTLN(powerConfig.val, BIN);

    // save power configuration to EEPROM
    Power_Control_Save_Configuration();

    // try to switch MPPT on (may be overridden by temperature check)
    Power_Control_Charge(true);

    // update timestamp
    lastBatteryCheck = millis();
  }

  // set to sleep for variable interval
  if(millis() - lastSleep >= SLEEP_PERIOD) {
    uint32_t interval = Power_Control_Get_Sleep_Interval();
    FOSSASAT_DEBUG_PRINT(F("Sleep for "));
    FOSSASAT_DEBUG_PRINTLN(interval);
    FOSSASAT_DEBUG_DELAY(100);
    Power_Control_Delay(interval, true);

    // update timestamp
    lastSleep = millis();
  }

  // pet watchdog every second
  if(millis() - lastHeartbeat >= WATCHDOG_LOOP_HEARTBEAT_PERIOD) {
    Pin_Interface_Watchdog_Heartbeat();
    lastHeartbeat = millis();
  }
}
