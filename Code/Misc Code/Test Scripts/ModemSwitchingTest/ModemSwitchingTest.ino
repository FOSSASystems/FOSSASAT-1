#include <RadioLib.h>
#include <TimerOne.h>

SX1268 radio = new Module(10, 2, 3, 9);

volatile byte currentModem = 0;
volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;
unsigned long lastTransmit = 0;

void changeModem(void) {
  if(!enableInterrupt) {
    return;
  }

  switch(currentModem) {
    case 0:
      radio.begin();
      currentModem++;
      break;
    case 1:
      radio.beginFSK();
      currentModem = 0;
      break;
  }

  radio.startReceive();
}

void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }

  receivedFlag = true;
}

void setup() {
  Serial.begin(9600);

  radio.beginFSK();
  radio.setTCXO(1.6);
  radio.setDio1Action(setFlag);
  radio.startReceive();

  Timer1.initialize(10000000);
  Timer1.attachInterrupt(changeModem);
}

void loop() {
  // send packet once every 5 seconds
  if(millis() - lastTransmit > 5000) {
    // disable timer and receive interrupts to transmit
    enableInterrupt = false;
    detachInterrupt(digitalPinToInterrupt(2));
  
    // send stuff by LoRa
    radio.begin();
    Serial.print("Transmitting ... ");
    radio.transmit("I'm transmitting now!");
    Serial.println("done!");
  
    // transmission done, enable interrupts
    radio.setDio1Action(setFlag);
    radio.startReceive();
    enableInterrupt = true;

    // update timestamp
    lastTransmit = millis();
  }

  if(receivedFlag) {
    enableInterrupt = false;
    receivedFlag = false;

    String str;
    int state = radio.readData(str);

    if (state == ERR_NONE) {
      Serial.println(F("Received packet!"));
      Serial.print(F("Data:\t\t"));
      Serial.println(str);

    } else {
      Serial.print(F("Failed, code "));
      Serial.println(state);

    }

    enableInterrupt = true;
  }
}
