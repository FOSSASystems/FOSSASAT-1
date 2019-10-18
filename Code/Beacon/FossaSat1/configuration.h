#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#include "FossaSat1.h"

// string length limit
#define MAX_STRING_LENGTH                               32

// message length limit
#define MAX_MESSAGE_LENGTH                              64

// debug macros - only comment out for debug purposes!

// comment out to disable transmission control (transmission disable and no transmissions in low power mode)
#define ENABLE_TRANSMISSION_CONTROL

// comment out to disable deployment sequence
//#define ENABLE_DEPLOYMENT_SEQUENCE

// comment out to disable automatic sleep interval control
#define ENABLE_INTERVAL_CONTROL

// comment out to skip INA226 reading
#define ENABLE_INA226

// uncomment to wipe EEPROM on start (will also set all EEPROM variables to defaults)
//#define EEPROM_WIPE


// power management configuration

// automatic battery voltage measurement period
#define BATTERY_CHECK_PERIOD                            300000 // ms

// satellite will go to sleep each time this period has elapsed
#define SLEEP_PERIOD                                    30000 // ms

// battery voltage limit to enable low power mode
#define BATTERY_VOLTAGE_LIMIT                           3.8f // V

// battery charging temperature limit
#define BATTERY_TEMPERATURE_LIMIT                       -0.7f // deg. C

// wachdog heartbeat period in loop()
#define WATCHDOG_LOOP_HEARTBEAT_PERIOD                  1000 // ms

// number of deployment attempts
#define DEPLOYMENT_ATTEMPTS                             2

// sleep for this period of time before deployment
#define DEPLOYMENT_SLEEP_LENGTH                         180000 // ms

// default power configuration
#define LOW_POWER_MODE_ACTIVE                           0
#define LOW_POWER_MODE_ENABLED                          1
#define MPPT_TEMP_SWITCH_ENABLED                        1
#define MPPT_KEEP_ALIVE_ENABLED                         0
#define TRANSMIT_ENABLED                                1

// INA226 configuration

// I2C address
#define INA_ADDR                                        0x40

// timeout
#define INA_TIMEOUT                                     2000 // ms

// shunt resistor value
#define INA_RSHUNT                                      0.1f // ohm

// maximum current
#define INA_MAX_CURRENT                                 0.5f // A

// registers
#define INA_REG_MANUFACTURER_ID                         0xFE
#define INA_MANUFACTURER_ID                             0x5449


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
#define ANALOG_IN_SOLAR_A_VOLTAGE_PIN                   A0  // PC0
#define ANALOG_IN_SOLAR_B_VOLTAGE_PIN                   A7  // ADC7
#define ANALOG_IN_SOLAR_C_VOLTAGE_PIN                   A2  // PC2
#define ANALOG_IN_RANDOM_SEED                           A6  // ADC6; used as source for randomSeed(), should be left floating
#define DIGITAL_OUT_MPPT_PIN                            10  // PB2
#define DIGITAL_OUT_MOSFET_1                            9   // PB1
#define DIGITAL_OUT_MOSFET_2                            8   // PB0
#define DIGITAL_OUT_WATCHDOG_HEARTBEAT                  4   // PD4
#define RADIO_NSS                                       7   // PD7
#define RADIO_DIO1                                      2   // PD2
#define RADIO_DIO2                                      3   // PD3
#define RADIO_BUSY                                      6   // PD6


// TMP100 temperature sensor configuration
#define BOARD_TEMP_SENSOR_ADDR                          0b1001100
#define BATTERY_TEMP_SENSOR_ADDR                        0b1001011

#define TEMP_SENSOR_REG_CONFIG                          0b01

#define TEMP_SENSOR_RESOLUTION_9_BITS                   0b00000000  // 0.5 deg. C
#define TEMP_SENSOR_RESOLUTION_10_BITS                  0b00100000  // 0.25 deg. C
#define TEMP_SENSOR_RESOLUTION_11_BITS                  0b01000000  // 0.125 deg. C
#define TEMP_SENSOR_RESOLUTION_12_BITS                  0b01100000  // 0.0625 deg. C


// MCU temperature sensor configuration
#define MCU_TEMP_OFFSET                                 324.31f   // t = (raw - MCU_TEMP_OFFSET) / MCU_TEMP_COEFFICIENT
#define MCU_TEMP_COEFFICIENT                            1.22f     // empirical constants


// radio configuration

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
#define FSK_RX_BANDWIDTH                                20.0f   // kHz single-sideband
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
extern uint32_t lastBatteryCheck;
extern uint32_t lastSleep;
extern uint32_t lastHeartbeat;
extern uint32_t lastRtty;

// INA226 instance
extern INA226 ina;

// RadioLib instances
extern SX1268 radio;
extern RTTYClient rtty;

// transmission password
extern const char* password;

// encryption key
extern const uint8_t encryptionKey[];

void Configuration_Setup_Pins();

#endif
