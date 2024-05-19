#include <Arduino.h>
#include <SoftwareSerial.h>
#include "R200.h"



// Create SoftwareSerial object on pins 10 (RX) and 11 (TX)
SoftwareSerial mySerial(33, 32);
R200 rfidReader(mySerial, 115200);

uint8_t targetepc[12] = {0xE2, 0x00, 0x00, 0x17, 0x57, 0x0D, 0x01, 0x23, 0x06, 0x30, 0xD6, 0x8E};
                

unsigned int freq = 220;

unsigned long tagLastSeen = millis();
unsigned long lastMultiplePollMessage = millis();


void setup() {
    // Begin serial communication with the R200 module
    Serial.begin(115200);
    Serial.println("Hello world.");


    rfidReader.begin();

    Serial.print("Begin serial communication with the R200 module");
    rfidReader.setTransmitPower(20);

    // Poll for tags
    rfidReader.initiateMultiplePolling(10000);

    Serial.print("initiated multiple polling.");


}

void loop() {
    uint8_t responseBuffer[256];
    //rfidReader.initiateSinglePolling();
   


    // Attempt to read a tag's EPC
    if (rfidReader.getResponse(responseBuffer, 256)) {

        if(rfidReader.hasValidTag(responseBuffer)) {

          uint8_t rssi;
          uint8_t epc[12];
          rfidReader.parseTagResponse(responseBuffer, rssi, epc);


          Serial.print("RSSI: ");
          Serial.print(rssi, DEC);
          Serial.print(" ");

          Serial.print("EPC: ");
          for (int i = 0; i < 12; ++i) {
              if (epc[i] < 0x10) {
                  Serial.print("0");  // Pad single hex digit with leading zero
              }
              Serial.print(epc[i], HEX);
              Serial.print(" ");
          }
          Serial.println(" ");


        }
    } else {
        Serial.println("No response received");
        
    }


    // Delay before the next loop iteration
    delay(10);

    if(millis() - lastMultiplePollMessage > 10000) {
      lastMultiplePollMessage = millis();
      rfidReader.initiateMultiplePolling(10000);
      Serial.println("initiating multiple polling");
    }
}
