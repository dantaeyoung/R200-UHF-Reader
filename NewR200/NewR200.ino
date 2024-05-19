#include <Arduino.h>
#include <SoftwareSerial.h>
#include "R200.h"
#include "DacESP32.h"

DacESP32 dac1(GPIO_NUM_25);



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

    dac1.outputCW(freq);
    Serial.println("Speaker on");
  

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
    parseSerialInput();


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

          if(memcmp(epc, targetepc, 12) == 0) {
            Serial.println("MATCH!!!");
            tagLastSeen = millis();
            dac1.outputCW(freq * 2 + map(rssi, 220, 190, freq, 0));
          } 

        }
    } else {
        Serial.println("No response received");
        dac1.outputCW(freq);

    }

    if (millis() - tagLastSeen > 300) {    
      dac1.outputCW(freq);
    }


    // Delay before the next loop iteration
    delay(10);

    if(millis() - lastMultiplePollMessage > 10000) {
      lastMultiplePollMessage = millis();
      rfidReader.initiateMultiplePolling(10000);
      Serial.println("initiating multiple polling");
    }
}


void parseSerialInput() {
    // Wait for input from the Serial monitor
    if(Serial.available()) {
         
      // Read the input string from the Serial monitor
      String input = Serial.readStringUntil('\n');
  
       // Convert the String to a char array
      char inputArray[input.length() + 1];
      input.toCharArray(inputArray, input.length() + 1);
  
      // Check for "hello" input
      if (strcmp(inputArray, "hello") == 0) {
          Serial.println("World");
          return;
      }

      // Check for "hello" input
      if (strcmp(inputArray, "mpoll") == 0) {
          Serial.println("MPOLLING 10000");
          rfidReader.initiateMultiplePolling(10000);
          return;
      }



      // Check if the input starts with "TARGETEPC:"
      if (strncmp(inputArray, "TARGETEPC:", 10) == 0) {
          // Remove the "TARGETEPC:" prefix
          char* epcString = inputArray + 10;
          while (*epcString == ' ') epcString++;  // Trim leading spaces
  
          // Split the input string by spaces and store in EPC buffer
          int index = 0;
          char* token = strtok(epcString, " ");
          while (token != NULL && index < 12) {
              // Convert the token to a uint8_t and store it in the EPC buffer
              targetepc[index++] = (uint8_t) strtol(token, NULL, 16);
              // Get the next token
              token = strtok(NULL, " ");
          }
  
          // Ensure we have exactly 12 bytes
          if (index == 12) {
              Serial.print("Stored EPC: ");
              for (int i = 0; i < 12; ++i) {
                  if (targetepc[i] < 0x10) {
                      Serial.print("0");  // Pad single hex digit with leading zero
                  }
                  Serial.print(targetepc[i], HEX);
                  Serial.print(" ");
              }
              Serial.println();
          } else {
              Serial.println("Error: EPC should contain exactly 12 bytes.");
          }
          
      } else {
          Serial.println("Invalid input.");
      }
    }
}
