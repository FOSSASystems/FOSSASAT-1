#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#include "Simulator.h"

// string length limit
#define MAX_STRING_LENGTH                               32

// message length limit
#define MAX_MESSAGE_LENGTH                              64


// EEPROM configuration

// EEPROM reset value
#define EEPROM_RESET_VALUE                              0xFF

// EEPROM address map                                             LSB       MSB
#define EEPROM_DEPLOYMENT_COUNTER_ADDR                  0x0000 // 0x0000    0x0000
#define EEPROM_POWER_CONFIG_ADDR                        0x0001 // 0x0001    0x0001
#define EEPROM_FIRST_RUN_ADDR                           0x0002 // 0x0002    0x0002
#define EEPROM_RESTART_COUNTER_ADDR                     0x0003 // 0x0003    0x0004
#define EEPROM_CALLSIGN_LEN_ADDR                        0x0005 // 0x0005    0x0005
#define EEPROM_CALLSIGN_ADDR                            0x0006 // 0x0006    0x0026

// EEPROM variables
//EEPROM_FIRST_RUN_ADDR
#define EEPROM_FIRST_RUN                                0
#define EEPROM_CONSECUTIVE_RUN                          1


// pin mapping
#define ANALOG_IN_SOLAR_A_VOLTAGE_PIN                   A0
#define ANALOG_IN_SOLAR_B_VOLTAGE_PIN                   A7
#define ANALOG_IN_SOLAR_C_VOLTAGE_PIN                   A2
#define ANALOG_IN_RANDOM_SEED                           A6  // used as source for randomSeed(), should be left floating
#define RADIO_NSS                                       10
#define RADIO_DIO0                                      2
#define RADIO_DIO1                                      3
#define RADIO_BUSY                                      9


// MCU temperature sensor configuration
#define MCU_TEMP_OFFSET                                 324.31f   // t = (raw - MCU_TEMP_OFFSET) / MCU_TEMP_COEFFICIENT
#define MCU_TEMP_COEFFICIENT                            1.22f     // empirical constants


// radio configuration
#define RADIO_TYPE                                      SX1278    // type of radio module to be used
//#define RADIO_SX126X                                            // also uncomment this line when using SX126x!!!

// comment out to disable RTTY
#define RTTY_ENABLED

// common
#define SYNC_WORD                                       0x0F0F
#define TCXO_VOLTAGE                                    1.6f    // V
#define LOW_POWER_LEVEL                                 10      // dBm
#define MAX_NUM_OF_BLOCKS                               3       // maximum number of AES128 blocks that will be accepted
#define MODEM_SWITCHING_PERIOD_FSK                      2000000 // us
#define MODEM_SWITCHING_PERIOD_LORA_ISM                 2000000 // us
#define MODEM_SWITCHING_PERIOD_LORA_NON_ISM             5000000 // us
#define SYSTEM_INFO_TRANSMIT_PERIOD                     30000   // ms
#define RTTY_SYSTEM_INFO_PERIOD                         180000  // ms

// LoRa
#define LORA_CARRIER_FREQUENCY_ISM                      434.5f  // MHz
#define LORA_CARRIER_FREQUENCY                          436.7f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_SPREADING_FACTOR_ALT                       10
#define LORA_CODING_RATE                                8       // 4/8, Extended Hamming
#define LORA_OUTPUT_POWER                               21      // dBm
#define LORA_CURRENT_LIMIT                              120     // mA

// Non-ISM band FSK
#define FSK_CARRIER_FREQUENCY                           436.7f  // MHz
#define FSK_BIT_RATE                                    1.2f    // kbps nominal
#define FSK_FREQUENCY_DEVIATION                         5.0f    // kHz single-sideband
#define FSK_RX_BANDWIDTH                                19.5f   // kHz single-sideband
#define FSK_OUTPUT_POWER                                21      // dBm
#define FSK_PREAMBLE_LENGTH                             16      // bits
#define FSK_DATA_SHAPING                                0.5f    // GFSK filter BT product
#define FSK_CURRENT_LIMIT                               120     // mA

// RTTY configuration
#define RTTY_PREAMBLE                                   "RY"
#define RTTY_PREAMBLE_LENGTH                            8       // number of RTTY_PREAMBLE repetitions
#define RTTY_FREQUENCY_SHIFT                            182     // Hz
#define RTTY_BAUDRATE                                   45      // baud
#define RTTY_ENCODING                                   ITA2    // 5-bit "Baudot" encoding

// modem definitions
#define MODEM_FSK_NON_ISM                               0
#define MODEM_LORA_ISM                                  1
#define MODEM_LORA_NON_ISM                              2

// spreading factor modes
#define SPREADING_FACTOR_STANDARD                       0
#define SPREADING_FACTOR_ALTERNATIVE                    1

// global variables

// flag to signal interrupts enabled/disabled
extern volatile bool interruptsEnabled;

// flag to signal data was received from ISR
extern volatile bool dataReceived;

// flag to signal modem should be switched
extern volatile bool switchModem;

// current modem configuration
extern uint8_t currentModem;

// current spreading factor mode
extern uint8_t spreadingFactorMode;

// timestamps
extern uint32_t lastTransmit;
extern uint32_t lastRtty;

// RadioLib instances
extern RADIO_TYPE radio;
extern RTTYClient rtty;

void Configuration_Setup_Pins();

#endif
