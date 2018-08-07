#include <Wire.h>

void requestEvent()
{
  Wire.write("6;");
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin(8);
  Serial.begin(9600);
  Wire.onRequest(requestEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.write("6;");
  delay(1000);
}
