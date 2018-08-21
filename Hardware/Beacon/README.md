# Beacon Explanation
The beacon is composed of an Atmega 328P AU custom circuit with a LoRa SX1278, a MAX6369 watchdog, 2 voltage dividers for voltage measurement, a current sensor to monitor battery charge and 2 mosfets to deploy the solar cells & antenna. The  plan is to transmit solar panel voltages, battery voltage, charge current, amount of resets and a satellite identification at 100 Baud, 100mw when nominal. The transmit power & frequency will be determined by battery voltage. The beacon must be able to be turned off and will be used to deploy the antenna and solar panels along with the payload's cpu itself for redundancy using a timer, the panels can be deployed with an uplink command incase of emergency.
Uplink can be either LORA or FSK and downlink is exclusively LORA and RTYY/MORSE.
Uplink for the General user will mostly be Ping Pong communication via LoRa.

