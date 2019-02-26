////////////////////////////////////////////////////////////////
///////////////////DMP STABILIZED DATA//////////////////////////
////////////////////////////////////////////////////////////////
//OPEN SERIAL MONITOR FOR VISUAL REPRESENTATION OF ORIENTATION//
////////////////////////////////////////////////////////////////

#include "MPU6050_6Axis_MotionApps20.h"
MPU6050 mpu(0x69); // <-- use for AD0 high

uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];

void setup() {
  Wire.begin();
  TWBR = 24;
  mpu.initialize();
  mpu.dmpInitialize();
  mpu.setXAccelOffset(-1343);
  mpu.setYAccelOffset(-1155);
  mpu.setZAccelOffset(1033);
  mpu.setXGyroOffset(19);
  mpu.setYGyroOffset(-27);
  mpu.setZGyroOffset(16);
  mpu.setDMPEnabled(true);
  packetSize = mpu.dmpGetFIFOPacketSize();
  fifoCount = mpu.getFIFOCount();

  Serial.begin(115200);
}

void loop() {
  while (fifoCount < packetSize) {
    fifoCount = mpu.getFIFOCount();
  }

  if (fifoCount == 1024) {
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));
  }
  else {
    if (fifoCount % packetSize != 0) {
      mpu.resetFIFO();
    }
    else {
      while (fifoCount >= packetSize) {
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;
      }

      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

      Serial.print("ypr\t");
      Serial.print(ypr[0] * 180 / PI);
      Serial.print("\t");
      Serial.print(ypr[1] * 180 / PI);
      Serial.print("\t");
      Serial.print(ypr[2] * 180 / PI);
      Serial.println();
    }
  }
}

