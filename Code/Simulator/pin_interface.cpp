#include "pin_interface.h"

int8_t Pin_Interface_Read_Temperature_Internal() {
  // select temperature sensor and reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  // start AD conversion
  ADCSRA |= _BV(ADEN) | _BV(ADSC);
  // Detect end-of-conversion
  while (bit_is_set(ADCSRA, ADSC));
  // get raw data
  uint16_t raw =  ADCL | (ADCH << 8);

  // convert to real temperature
  int8_t temp = (int8_t)(((float)raw - MCU_TEMP_OFFSET) / MCU_TEMP_COEFFICIENT);

  FOSSASAT_DEBUG_PRINT(F("MCU temp: "));
  FOSSASAT_DEBUG_PRINTLN(temp);
  return(temp);
}

float Pin_Interface_Read_Voltage(uint8_t pin) {
  // map ADC value to voltage
  return((analogRead(pin) * 3.3) / 1023.0);
}
