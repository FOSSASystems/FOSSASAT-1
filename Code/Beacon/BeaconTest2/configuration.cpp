#include "configuration.h"

// flag to signal interrupts enabled/disabled
volatile bool interruptsEnabled = true;

// flag to signal data was received from ISR
volatile bool dataReceived = false;

// flag to signal modem should be switched
volatile bool switchModem = false;

// current modem configuration
uint8_t currentModem = MODEM_FSK_NON_ISM;

// current spreading factor mode
uint8_t spreadingFactorMode = SPREADING_FACTOR_STANDARD;

// timestamps
uint32_t lastTransmit = 0;
uint32_t lastBatteryCheck = 0;
uint32_t lastSleep = 0;
uint32_t lastHeartbeat = 0;
uint32_t lastRtty = 0;

// INA226 instance
INA226 ina;

// RadioLib instances
SX1268 radio = new Module(RADIO_NSS, RADIO_DIO1, RADIO_DIO2, RADIO_BUSY);
RTTYClient rtty(&radio);

// transmission password
const char* password = "password";

// encryption key
const uint8_t encryptionKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

void Configuration_Setup_Pins() {
  // set up digital pins
  pinMode(DIGITAL_OUT_MOSFET_1, OUTPUT);
  pinMode(DIGITAL_OUT_MOSFET_2, OUTPUT);
  pinMode(DIGITAL_OUT_WATCHDOG_HEARTBEAT, OUTPUT);
  pinMode(DIGITAL_OUT_MPPT_PIN, OUTPUT);

  // set up analog pins
  pinMode(ANALOG_IN_SOLAR_A_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_SOLAR_B_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_SOLAR_C_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_RANDOM_SEED, INPUT);

  // bring deployment MOSFETs low
  digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
  digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

  // provide seed for encrpytion PRNG
  randomSeed(analogRead(ANALOG_IN_RANDOM_SEED));
}
