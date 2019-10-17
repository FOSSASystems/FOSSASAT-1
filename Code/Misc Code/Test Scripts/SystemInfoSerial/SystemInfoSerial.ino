// libraries
#include <INA226.h>
#include <Wire.h>
#include <EEPROM.h>

// pin mapping
#define ANALOG_IN_SOLAR_A_VOLTAGE_PIN                   A0  // PC0
#define ANALOG_IN_SOLAR_B_VOLTAGE_PIN                   A7  // ADC7
#define ANALOG_IN_SOLAR_C_VOLTAGE_PIN                   A2  // PC2
#define DIGITAL_OUT_MPPT_PIN                            10  // PB2

// EEPROM address map                                             LSB       MSB
#define EEPROM_POWER_CONFIG_ADDR                        0x0002 // 0x0002    0x0002
#define EEPROM_RESTART_COUNTER_ADDR                     0x0004 // 0x0004    0x0005

// TMP100 temperature sensor configuration
#define BOARD_TEMP_SENSOR_ADDR                          0b1001011
#define BATTERY_TEMP_SENSOR_ADDR                        0b1001011

#define TEMP_SENSOR_REG_CONFIG                          0b01

#define TEMP_SENSOR_RESOLUTION_9_BITS                   0b00000000  // 0.5 deg. C
#define TEMP_SENSOR_RESOLUTION_10_BITS                  0b00100000  // 0.25 deg. C
#define TEMP_SENSOR_RESOLUTION_11_BITS                  0b01000000  // 0.125 deg. C
#define TEMP_SENSOR_RESOLUTION_12_BITS                  0b01100000  // 0.0625 deg. C

// INA226 configuration

// I2C address
#define INA_ADDR                                        0x40

// shunt resistor value
#define INA_RSHUNT                                      0.1f // ohm

// maximum current
#define INA_MAX_CURRENT                                 0.5f // A

// INA226 instance
INA226 ina;

template <class T>
T Persistent_Storage_Read(uint16_t addr) {
  T val;
  EEPROM.get(addr, val);
  return(val);
}

template uint8_t Persistent_Storage_Read<uint8_t>(uint16_t);
template uint16_t Persistent_Storage_Read<uint16_t>(uint16_t);

void Pin_Interface_Set_Temp_Resolution(uint8_t sensorAddr, uint8_t res) {
  // set resolution
  Wire.beginTransmission(sensorAddr);
  Wire.write(TEMP_SENSOR_REG_CONFIG);
  Wire.write(res);
  Wire.endTransmission();

  // reset to reading the temperature
  Wire.beginTransmission(sensorAddr);
  Wire.write((uint8_t)0x00);
  Wire.endTransmission();
}

float Pin_Interface_Read_Temperature(uint8_t sensorAddr) {
  Wire.requestFrom(sensorAddr, (uint8_t)2);

  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();

  int16_t tempRaw = 0;
  tempRaw = ((msb << 8) | lsb) >> 4;

  return(tempRaw * 0.0625f);
}

void setup() {
  Serial.begin(9600);

  // set up digital pins
  pinMode(DIGITAL_OUT_MPPT_PIN, OUTPUT);

  // set up analog pins
  pinMode(ANALOG_IN_SOLAR_A_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_SOLAR_B_VOLTAGE_PIN, INPUT);
  pinMode(ANALOG_IN_SOLAR_C_VOLTAGE_PIN, INPUT);


  Pin_Interface_Set_Temp_Resolution(BOARD_TEMP_SENSOR_ADDR, TEMP_SENSOR_RESOLUTION_12_BITS);
  Pin_Interface_Set_Temp_Resolution(BATTERY_TEMP_SENSOR_ADDR, TEMP_SENSOR_RESOLUTION_12_BITS);

  ina.begin(INA_ADDR);
  ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina.calibrate(INA_RSHUNT, INA_MAX_CURRENT);
}

void loop() {
  Serial.print("batteryChargingVoltage = ");
  Serial.println(ina.readBusVoltage());
  
  Serial.print("batteryChargingCurrent = ");
  Serial.println(ina.readShuntCurrent());
  
  Serial.print("batteryVoltage = ");
  digitalWrite(DIGITAL_OUT_MPPT_PIN, LOW);
  Serial.println(ina.readBusVoltage());
  digitalWrite(DIGITAL_OUT_MPPT_PIN, HIGH);
  
  Serial.print("solarCellAVoltage = ");
  Serial.println((analogRead(ANALOG_IN_SOLAR_A_VOLTAGE_PIN) * 3.3) / 1023.0);
  
  Serial.print("solarCellBVoltage = ");
  Serial.println((analogRead(ANALOG_IN_SOLAR_B_VOLTAGE_PIN) * 3.3) / 1023.0);
  
  Serial.print("solarCellCVoltage = ");
  Serial.println((analogRead(ANALOG_IN_SOLAR_C_VOLTAGE_PIN) * 3.3) / 1023.0);
  
  Serial.print("batteryTemperature = ");
  Serial.println(Pin_Interface_Read_Temperature(BATTERY_TEMP_SENSOR_ADDR));
  
  Serial.print("boardTemperature = ");
  Serial.println(Pin_Interface_Read_Temperature(BOARD_TEMP_SENSOR_ADDR));
  
  Serial.print("resetCounter = ");
  Serial.println(Persistent_Storage_Read<uint16_t>(EEPROM_RESTART_COUNTER_ADDR));
  
  Serial.print("powerConfig = 0b");
  Serial.println(Persistent_Storage_Read<uint8_t>(EEPROM_RESTART_COUNTER_ADDR), BIN);

  Serial.println("----------------------------------");
  delay(1000);
}
