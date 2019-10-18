#include "power_control.h"

powerConfigBits_t powerConfigBits = {
  .lowPowerModeActive = LOW_POWER_MODE_ACTIVE,
  .lowPowerModeEnabled = LOW_POWER_MODE_ENABLED,
  .mpptTempSwitchEnabled = MPPT_TEMP_SWITCH_ENABLED,
  .mpptKeepAliveEnabled = MPPT_KEEP_ALIVE_ENABLED,
  .transmitEnabled = TRANSMIT_ENABLED
};

powerConfig_t powerConfig = {
  .bits = powerConfigBits
};

void Power_Control_Load_Configuration() {
  powerConfig.val = Persistent_Storage_Read<uint8_t>(EEPROM_POWER_CONFIG_ADDR);
}

void Power_Control_Save_Configuration() {
  Persistent_Storage_Write<uint8_t>(EEPROM_POWER_CONFIG_ADDR, powerConfig.val);
}

void Power_Control_Charge(bool charge) {
  FOSSASAT_DEBUG_PRINT(F("MPPT "));
  FOSSASAT_DEBUG_PRINTLN(charge);

  Power_Control_Load_Configuration();
  if(powerConfig.bits.mpptKeepAliveEnabled) {
    // force MPPT to float regardless of anything else
    FOSSASAT_DEBUG_PRINTLN(F("Keep alive"));
    pinMode(DIGITAL_OUT_MPPT_PIN, INPUT);
  } else if((Pin_Interface_Read_Temperature(BATTERY_TEMP_SENSOR_ADDR) < BATTERY_TEMPERATURE_LIMIT) && powerConfig.bits.mpptTempSwitchEnabled) {
    // force MPPT low, only if temperature switch is enabled
    FOSSASAT_DEBUG_PRINTLN(F("Low temp"));
    pinMode(DIGITAL_OUT_MPPT_PIN, OUTPUT);
    digitalWrite(DIGITAL_OUT_MPPT_PIN, LOW);
  } else if(charge){
    // set MPPT to float
    pinMode(DIGITAL_OUT_MPPT_PIN, INPUT);
  } else {
    // set MPPT to low
    pinMode(DIGITAL_OUT_MPPT_PIN, OUTPUT);
    digitalWrite(DIGITAL_OUT_MPPT_PIN, LOW);
  }
}


uint32_t Power_Control_Get_Sleep_Interval() {
  // sleep interval in ms (default for battery > 3.7 V)
  uint32_t interval = 0;

  #ifdef ENABLE_INTERVAL_CONTROL
    // get battery voltage
    float batt = Power_Control_Get_Battery_Voltage();

    if(batt > 4.05f) {
      interval = (uint32_t)20 * (uint32_t)1000;
    } else if(batt > 4.0f) {
      interval = (uint32_t)35 * (uint32_t)1000;
    } else if(batt > 3.9f) {
      interval = (uint32_t)100 * (uint32_t)1000;
    } else if(batt > 3.8f) {
      interval = (uint32_t)160 * (uint32_t)1000;
    } else if(batt > 3.7f) {
      interval = (uint32_t)180 * (uint32_t)1000;
    } else {
      interval = (uint32_t)240 * (uint32_t)1000;
    }
  #endif

  return(interval);
}

void Power_Control_Delay(uint32_t ms, bool sleep) {
  if(ms == 0) {
    return;
  }

  // calculate number of required loops (rounded up)
  float numLoops = 0.5f;
  if(sleep) {
    numLoops += (float)ms / 1000.0;
  } else {
    numLoops += (float)ms / 50.0;
  }

  // perform all loops
  for(uint32_t i = 0; i < (uint32_t)numLoops; i++) {
    Pin_Interface_Watchdog_Heartbeat();

    if(sleep) {
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    } else {
      delay(50);
    }
  }
}

void Power_Control_Setup_INA226() {
  FOSSASAT_DEBUG_PRINTLN(F("INA setup "));

  ina.begin(INA_ADDR);
  ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina.calibrate(INA_RSHUNT, INA_MAX_CURRENT);
}

bool Power_Control_INA2256_Check() {
  // attempt to read manufacturer ID register
  Wire.beginTransmission(INA_ADDR);
  Wire.write(INA_REG_MANUFACTURER_ID);
  Wire.endTransmission();
  delay(1);

  // try to read
  Wire.beginTransmission(INA_ADDR);
  Wire.requestFrom((uint8_t)INA_ADDR, (uint8_t)2);
  uint32_t start = millis();
  while(!Wire.available()) {
    if(millis() - start >= INA_TIMEOUT) {
      // timed out
      return(false);
    }
  }

  // check value
  uint8_t vha = Wire.read();
  uint8_t vla = Wire.read();
  uint16_t value = vha << 8 | vla;

  if(value != INA_MANUFACTURER_ID) {
    return(false);
  }

  return(true);
}

float Power_Control_Get_Battery_Voltage() {
  // try to switch MPPT off (may be overridden by MPPT keep alive)
  Power_Control_Charge(false);

  // get voltage
  float val = -999;
  if(Power_Control_INA2256_Check()) {
    val = ina.readBusVoltage();
  }

  // try to switch MPPT on (may be overridden by temperature check)
  Power_Control_Charge(true);

  return(val);
}

float Power_Control_Get_Charging_Voltage() {
  if(!Power_Control_INA2256_Check()) {
    return(-999.0);
  }
  return(ina.readBusVoltage());
}

float Power_Control_Get_Charging_Current() {
  if(!Power_Control_INA2256_Check()) {
    return(-999.0);
  }
  return(ina.readShuntCurrent());
}
