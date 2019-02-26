/*
   LoRaLib Transmit Example

   This example transmits LoRa packets with BMP and Magnetometer information with 6 second intervals.
*/

// include the library
#include <LoRaLib.h>

// BMP280

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

//COMPASS 

#include <QMC5883L.h>

// configure the compass as reqiured
#define OSR 0b00               // over sampling rate set to 512. 0b01 is 256, 0b10 is 128 and 0b11 is 64
#define RNG 0b00               // Full Scale set to +/- 2 Gauss, 0b01 is +/- 8.
#define ODR 0b00               // output data rate set to 10Hz, 0b01 is 50Hz, 0b10 is 100Hz, 0b11 is 200Hz
#define MODE 0b01              // continuous measurement mode, 0b00 is standby
#define CR2 0b00000000          // control register 2: disable soft reset and pointer rollover, interrupt pin not enabled
#define RESETPERIOD 0b00000001  // datasheet recommends this be 1, not sure why!

// Store our compass as a variable.

QMC5883L compass;

// Record any errors that may occur in the compass.

int error = 0;

//BMP280
Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

// create instance of LoRa class using SX1278 module
// this pinout corresponds to LoRenz shield:
// https://github.com/jgromes/LoRenz
// NSS pin:   7 (18 on ESP32 boards)
// DIO0 pin:  2
// DIO1 pin:  3
SX1278 lora = new LoRa;

void setup() {
  Serial.begin(9600);

  //COMPASS SETUP
    Serial.println("Starting the I2C interface.");
  Wire.begin(); // Start the I2C interface.
  TWBR = 12;    //Set the I2C clock speed to 400kHz - only works with Arduino UNO

  Serial.println("Constructing new QMC5883L");
  compass = QMC5883L(); // Construct a new HMC5883 compass.

  // Check that a device responds at the compass address - don't continue if it doesn't - 
  do {
      delay(100);
      Wire.beginTransmission(QMC5883L_Address);
      error = Wire.endTransmission();
      if (error) Serial.println("Can't find compass - is it connected and powered up?");
  } while (error);

  // configure the control registers using static settings above
  // compass autoranges, but starts in the mode given
  compass.dataRegister.OSR_RNG_ODR_MODE = (OSR << 6) |(RNG << 4)  | (ODR <<2) |  MODE;
  compass.dataRegister.CR2_INT_ENABLE = CR2;
  compass.dataRegister.SET_RESET_PERIOD = RESETPERIOD;
    
  Serial.println("Configuring QMC5883L - OSR 512, range +/-2 Gauss, ODR 10, Continuous");
  error = compass.Configure(compass.dataRegister); // use static settings from above - can access register data directly if required..
  if(error != 0) // If there is an error, print it out, although no way to get error with this sensor....
    Serial.println(compass.GetErrorText(error));
  
  //INITIAL BMP SETUP
  
if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
     }
    //BMP SETTINGS
    
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
                  
  // initialize SX1278 with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            11
  // coding rate:                 8
  // sync word:                   0x12
  // output power:                10dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  int state = lora.begin(434.0, 125.0, 11, 8, 0x12, 10);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  //////////////////////////////////////////////////////////////COMPASS////////////////////////////////////////////////////////
  // Retrive the raw values from the compass (not scaled).
  MagnetometerRaw raw = compass.ReadRawAxis(&compass.dataRegister);
  // Retrived the scaled values from the compass (scaled to the configured scale).
  MagnetometerScaled scaled = compass.ReadScaledAxis(&compass.dataRegister);
  
  // Values are accessed like so:
  int Gauss_OnThe_XAxis = scaled.XAxis;     // (or YAxis, or ZAxis)

  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  // relative to a magnetometer mounted with 'Y' axis pointing in the direction to be measured
  // and with 'X' pointing to the right from the perspective of the operator facing in the +Y direction
  // heading (degrees): 0 = +X, 90 = +Y, 180 = -X, 270 = -Y
  float heading = atan2(scaled.YAxis, scaled.XAxis);
 
  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Example is: 2� 37' W, which is 2.617 Degrees, or (which we need) 0.0456752665 radians, I will use 0.0457
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  // float declinationAngle = 0.0457;
  float declinationAngle = 0;  // compass reads magnetic north for those who remember how to read maps and adjust for declination
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 

  // Output the data via the serial port.
  Output(raw, scaled, heading, headingDegrees);

  // Normally we would either:
  // 1. delay the application by 100ms to allow the loop to run at 10Hz (default bandwidth for the QMC5883L)
  // 2. poll the dataready flag in the dataRegister.OVL_DRDY register
  // 3. set the interrupt flat and set a hardware interrupt on the DRDY pin 
  // The first of these options is the easiest.
  delay(100);
}

// Output the data down the serial port.
void Output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees)
{
////////////////////////////////////////////////////////////////////TRANSMISSION///////////////////////////////
  Serial.print("Sending packet ... ");

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  String sysInfoMessage = String("");
  sysInfoMessage += String("SAT-1,") + bmp.readTemperature();
  sysInfoMessage += String("C,") + bmp.readAltitude(1013.25);
  sysInfoMessage += String("M,") + headingDegrees;
  sysInfoMessage += String("DEG") + ";";
  
  int state = lora.transmit(sysInfoMessage);
Serial.println(sysInfoMessage);

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(" success!");

    // print measured data rate
    Serial.print("Datarate:\t");
    Serial.print(lora.dataRate);
    Serial.println(" bps");

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(" too long!");

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(" timeout!");

  }

  // wait 6 seconds before transmitting again (Duty cycle limitation)
  delay(6000);
}
