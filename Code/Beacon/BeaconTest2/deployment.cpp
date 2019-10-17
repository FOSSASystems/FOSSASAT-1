#include "deployment.h"

void Deployment_Deploy() {
  FOSSASAT_DEBUG_PRINTLN(F("Deploy"));

  // burn the nichrome wires
  digitalWrite(DIGITAL_OUT_MOSFET_1, HIGH);
  digitalWrite(DIGITAL_OUT_MOSFET_2, HIGH);

  // wait for 1 second
  Power_Control_Delay(1200, false);

  // set MOSFETs low
  digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
  digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

  // increment reset counter
  Persistent_Storage_Write<uint8_t>(EEPROM_DEPLOYMENT_COUNTER_ADDR, Persistent_Storage_Read<uint8_t>(EEPROM_DEPLOYMENT_COUNTER_ADDR) + 1);
}
