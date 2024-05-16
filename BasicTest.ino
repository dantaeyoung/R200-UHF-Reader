/*
Written by Sichuan Ruipin Technology Co., Ltd.
Copy and use, please retain copyright

NOTE: plug TX/RX into pins 0 and 1 (RX and TX) of an Arduino
*/

/* Read the command multiple times */
unsigned char ReadMulti[10] = {0XAA,0X00,0X27,0X00,0X03,0X22,0XFF,0XFF,0X4A,0XDD};
unsigned int timeSec = 0;
unsigned int timemin = 0;
unsigned int dataAdd = 0;
unsigned int incomedate = 0;
unsigned int parState = 0;
unsigned int codeState = 0;

void setup() {
  // Set the serial port and set the LED
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200); // Set the serial port baud rate to 115200
  
  Serial.println("Hello world.");
  
  Serial.write(ReadMulti, 10);
}

void loop() {
  // Loop read commands occur after some interval of time
  timeSec++;
  
  if (timeSec >= 50000) {
    timemin++;
    
    timeSec = 0;
    
    if (timemin >= 20) {
      timemin = 0;
      // Send loop read command
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.write(ReadMulti, 10);
      digitalWrite(LED_BUILTIN, LOW); 
    }
  }
 
  if (Serial.available() > 0) // The serial port receives data
  {
    // Received a response, the following is an example of reading a card
    // AA 02 22 00 11 C7 30 00 E2 80 68 90 00 00 50 0E 88 C6 A4 A7 11 9B 29 DD 
    /*
    AA: Frame header
    02: Command code
    22: Command parameter
    00 11: Command data length, 17 bytes
    C7: RSSI signal strength
    30 00: Tag PC code: factory-related information registration
    E2 80 68 90 00 00 50 0E 88 C6 A4 A7: EPC code
    11 9B: CRC check
    29: Check
    DD: Frame tail
    */
    
    incomedate = Serial.read(); // Get the data received by the serial port
    
    // Determine if it is the corresponding command code
    if ((incomedate == 0x02) & (parState == 0)) {
      parState = 1;
    }
    // Determine if it is the corresponding command parameter
    else if ((parState == 1) & (incomedate == 0x22) & (codeState == 0)) {  
      codeState = 1;
      dataAdd = 3;
    }
    else if (codeState == 1) {
      dataAdd++;
      
      // Get RSSI
      if (dataAdd == 6) {
        Serial.print("RSSI:"); 
        Serial.println(incomedate, HEX); 
      }
      // Get PC code
      else if ((dataAdd == 7) | (dataAdd == 8)) {
        if (dataAdd == 7) {
          Serial.print("PC:"); 
          Serial.print(incomedate, HEX);
        }
        else {
          Serial.println(incomedate, HEX);
        }
      }
      // Get EPC, if processing EPC is needed, it can be done here
      else if ((dataAdd >= 9) & (dataAdd <= 20)) {
        if (dataAdd == 9) {
          Serial.print("EPC:"); 
        }        
        Serial.print(incomedate, HEX);
      }
      // Location overflow, re-receive
      else if (dataAdd >= 21) {
        Serial.println(" "); 
        
        dataAdd = 0;
        parState = 0;
        codeState = 0;
      }
    }
    else {
      dataAdd = 0;
      parState = 0;
      codeState = 0;
    }
  }
}
