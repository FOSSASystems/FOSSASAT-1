#include "configuration.h"

// flag to signal interrupts enabled/disabled
volatile bool interruptsEnabled = true;

// flag to signal data was received from ISR
volatile bool dataReceived = false;

// flag to signal modem should be switched
volatile bool switchModem = false;

// current modem configuration
uint8_t currentModem = MODEM_LORA_NON_ISM;

// current spreading factor mode
uint8_t spreadingFactorMode = SPREADING_FACTOR_STANDARD;

// timestamps
uint32_t lastTransmit = 0;
uint32_t lastRtty = 0;

// RadioLib instances
#ifdef RADIO_SX126X
  RADIO_TYPE radio = new Module(RADIO_NSS, RADIO_DIO0, RADIO_BUSY);
#else
  RADIO_TYPE radio = new Module(RADIO_NSS, RADIO_DIO0, RADIO_DIO1);
#endif

RTTYClient rtty(&radio);

void Configuration_Setup_Pins() {
  // set up analog pins
  pinMode(ANALOG_IN_SOLAR_A_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_SOLAR_B_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_SOLAR_C_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_RANDOM_SEED, INPUT);

  // provide seed for encrpytion PRNG
  randomSeed(analogRead(ANALOG_IN_RANDOM_SEED));
}
