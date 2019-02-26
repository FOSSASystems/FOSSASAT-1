
//Voltage Division with resistor information returns actual voltage

const int voltPin1 = 0;
const int voltPin2 = 1;
const int voltPin3 = 2;
const int voltPin4 = 3;
const int voltPin5 = 6;

float denominator;

int resistor1 = 6800;

int resistor2 = 2200;

void setup() {

  Serial.begin(9600);

  denominator = (float)resistor2 / (resistor1 + resistor2);
} 

void loop() {

 float voltage1;
 
  float voltage2;
  
   float voltage3;
   
    float voltage4;
    
     float voltage5;
  voltage1 = analogRead(voltPin1);
 
  voltage1 = (voltage1 / 1024) * 3.3;

  voltage1 = voltage1 / denominator;
  
  voltage2 = analogRead(voltPin2);
 
  voltage2 = (voltage2 / 1024) * 3.3;

  voltage2 = voltage2 / denominator;
  
  voltage3 = analogRead(voltPin3);
 
  voltage3 = (voltage3 / 1024) * 3.3;

  voltage3 = voltage3 / denominator;
  
  voltage4 = analogRead(voltPin4);
 
  voltage4 = (voltage4 / 1024) * 3.3;

  voltage4 = voltage4 / denominator;

  
  voltage5 = analogRead(voltPin5);
 
  voltage5 = (voltage5 / 1024) * 3.3;

  voltage5 = voltage5 / denominator;
 
  
  
  
  
  Serial.print("Battery Voltage: ");
  Serial.println(voltage1);
  Serial.print("Solar Voltage: ");
  Serial.println(voltage2);
  Serial.print("A2 Voltage: ");
  Serial.println(voltage3);
  Serial.print("A3 Voltage: ");
  Serial.println(voltage4);
  Serial.print("A6 Voltage: ");
  Serial.println(voltage5);
  
  delay(500);
} 
