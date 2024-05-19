#include <Arduino.h>
#include <SoftwareSerial.h>
#include "R200.h"

// Create SoftwareSerial object on pins 10 (RX) and 11 (TX)
SoftwareSerial mySerial(26, 25);
R200 rfidReader(mySerial, 115200);




void setup() {
    // Begin serial communication with the R200 module
        Serial.begin(115200);
  Serial.println("Hello world.");
    rfidReader.begin();

    Serial.print("Begin serial communication with the R200 module");

    // Poll for tags
    //rfidReader.initiateMultiplePolling(100);

    Serial.print("initiated multiple polling.");


}

void loop() {
    uint8_t responseBuffer[256];
    rfidReader.initiateSinglePolling();

    // Attempt to read a tag's EPC
    if (rfidReader.getResponse(responseBuffer, 256)) {

        if(rfidReader.hasValidTag(responseBuffer)) {

          uint8_t rssi;
          uint8_t epc[12];
          rfidReader.parseTagResponse(responseBuffer, rssi, epc);

          Serial.print("RSSI: ");
          Serial.println(rssi, DEC);

          Serial.print("EPC: ");
          for (int i = 0; i < 12; ++i) {
              if (epc[i] < 0x10) {
                  Serial.print("0");  // Pad single hex digit with leading zero
              }
              Serial.print(epc[i], HEX);
              Serial.print(" ");
          }

        }
    } else {
        Serial.println("No response received");
    }

    // Delay before the next loop iteration
    delay(10);
}
