#include "R200.h"

// Constructor for HardwareSerial
R200::R200(HardwareSerial &serial, uint32_t baudRate) : serial(serial), baudRate(baudRate), isHardwareSerial(true) {}

// Constructor for SoftwareSerial
R200::R200(SoftwareSerial &serial, uint32_t baudRate) : serial(serial), baudRate(baudRate), isHardwareSerial(false) {}

void R200::begin() {
    if (isHardwareSerial) {
        ((HardwareSerial&)serial).begin(baudRate);
    } else {
        ((SoftwareSerial&)serial).begin(baudRate);
    }
}

void R200::setTransmitPower(uint16_t power) {
    uint8_t command[] = {0xAA, 0x00, 0xB6, 0x00, 0x02, (power >> 8) & 0xFF, power & 0xFF, 0x00, 0xDD};
    command[7] = calculateChecksum(command, 7);
    sendCommand(command, sizeof(command));
}

void R200::readTagData(uint32_t accessPassword, uint8_t memBank, uint16_t address, uint16_t length, uint8_t *data) {
    uint8_t command[] = {
        0xAA, 0x00, 0x39, 0x00, 0x09, 
        (accessPassword >> 24) & 0xFF, (accessPassword >> 16) & 0xFF, 
        (accessPassword >> 8) & 0xFF, accessPassword & 0xFF, 
        memBank, 
        (address >> 8) & 0xFF, address & 0xFF, 
        (length >> 8) & 0xFF, length & 0xFF, 
        0x00, 0xDD
    };
    command[14] = calculateChecksum(command, 14);
    sendCommand(command, sizeof(command));
    getResponse(data, length * 2 + 10);
}

void R200::writeTagData(uint32_t accessPassword, uint8_t memBank, uint16_t address, uint16_t length, uint8_t *data) {
    uint8_t command[18 + length * 2] = {
        0xAA, 0x00, 0x49, 0x00, static_cast<uint8_t>(9 + length * 2),
        (accessPassword >> 24) & 0xFF, (accessPassword >> 16) & 0xFF, 
        (accessPassword >> 8) & 0xFF, accessPassword & 0xFF, 
        memBank, 
        (address >> 8) & 0xFF, address & 0xFF, 
        (length >> 8) & 0xFF, length & 0xFF
    };
    for (uint16_t i = 0; i < length * 2; i++) {
        command[14 + i] = data[i];
    }
    command[14 + length * 2] = calculateChecksum(command, 14 + length * 2);
    command[15 + length * 2] = 0xDD;
    sendCommand(command, sizeof(command));
}

void R200::initiateSinglePolling() {
    uint8_t command[] = {0xAA, 0x00, 0x22, 0x00, 0x00, 0x22, 0xDD};
    sendCommand(command, sizeof(command));
}

void R200::initiateMultiplePolling(uint16_t count) {
    uint8_t command[] = {0xAA, 0x00, 0x27, 0x00, 0x03, 0x22, (count >> 8) & 0xFF, count & 0xFF, 0x00, 0xDD};
    command[8] = calculateChecksum(command, 8);
   for (int i = 0; i < 10; ++i) {
        if (command[i] < 0x10) {
          Serial.print("0");  // Pad single hex digit with leading zero
        }     
        Serial.print(command[i], HEX);
        Serial.print(" ");
    }
    sendCommand(command, sizeof(command));

}

void R200::stopMultiplePolling() {
    uint8_t command[] = {0xAA, 0x00, 0x28, 0x00, 0x00, 0x28, 0xDD};
    sendCommand(command, sizeof(command));
}

void R200::setSelectParameter(uint8_t target, uint8_t action, uint8_t memBank, uint32_t pointer, uint8_t maskLength, uint8_t *mask) {
    uint8_t command[22] = {
        0xAA, 0x00, 0x0C, 0x00, 0x13, 
        target, action, memBank, 
        (pointer >> 24) & 0xFF, (pointer >> 16) & 0xFF, 
        (pointer >> 8) & 0xFF, pointer & 0xFF, 
        maskLength, 0x00
    };
    memcpy(&command[14], mask, maskLength);
    command[14 + maskLength] = calculateChecksum(command, 14 + maskLength);
    command[15 + maskLength] = 0xDD;
    sendCommand(command, 16 + maskLength);
}

void R200::getSelectParameter(uint8_t *response, size_t length) {
    uint8_t command[] = {0xAA, 0x00, 0x0B, 0x00, 0x00, 0x0B, 0xDD};
    sendCommand(command, sizeof(command));
    getResponse(response, length);
}

void R200::killTag(uint32_t killPassword) {
    uint8_t command[] = {
        0xAA, 0x00, 0x65, 0x00, 0x04, 
        (killPassword >> 24) & 0xFF, (killPassword >> 16) & 0xFF, 
        (killPassword >> 8) & 0xFF, killPassword & 0xFF, 
        0x00, 0xDD
    };
    command[9] = calculateChecksum(command, 9);
    sendCommand(command, sizeof(command));
}

void R200::lockTag(uint32_t accessPassword, uint16_t lockPayload) {
    uint8_t command[] = {
        0xAA, 0x00, 0x82, 0x00, 0x07, 
        (accessPassword >> 24) & 0xFF, (accessPassword >> 16) & 0xFF, 
        (accessPassword >> 8) & 0xFF, accessPassword & 0xFF, 
        (lockPayload >> 8) & 0xFF, lockPayload & 0xFF, 
        0x00, 0xDD
    };
    command[11] = calculateChecksum(command, 11);
    sendCommand(command, sizeof(command));
}

void R200::setWorkArea(uint8_t region) {
    uint8_t command[] = {0xAA, 0x00, 0x07, 0x00, 0x01, region, 0x00, 0xDD};
    command[6] = calculateChecksum(command, 6);
    sendCommand(command, sizeof(command));
}

void R200::getWorkArea(uint8_t *response, size_t length) {
    uint8_t command[] = {0xAA, 0x00, 0x08, 0x00, 0x00, 0x08, 0xDD};
    sendCommand(command, sizeof(command));
    getResponse(response, length);
}

void R200::setWorkingChannel(uint8_t channelIndex) {
    uint8_t command[] = {0xAA, 0x00, 0xAB, 0x00, 0x01, channelIndex, 0x00, 0xDD};
    command[6] = calculateChecksum(command, 6);
    sendCommand(command, sizeof(command));
}

void R200::getWorkingChannel(uint8_t *response, size_t length) {
    uint8_t command[] = {0xAA, 0x00, 0xAA, 0x00, 0x00, 0xAA, 0xDD};
    sendCommand(command, sizeof(command));
    getResponse(response, length);
}

void R200::setAutomaticFrequencyHopping(bool enable) {
    uint8_t command[] = {0xAA, 0x00, 0xAD, 0x00, 0x01, enable ? 0xFF : 0x00, 0x00, 0xDD};
    command[6] = calculateChecksum(command, 6);
    sendCommand(command, sizeof(command));
}

void R200::insertWorkingChannel(uint8_t count, uint8_t *channelList) {
    uint8_t command[9 + count] = {0xAA, 0x00, 0xA9, 0x00, static_cast<uint8_t>(count + 1), count};
    memcpy(&command[6], channelList, count);
    command[6 + count] = calculateChecksum(command, 6 + count);
    command[7 + count] = 0xDD;
    sendCommand(command, 8 + count);
}

void R200::moduleSleep() {
    uint8_t command[] = {0xAA, 0x00, 0x17, 0x00, 0x00, 0x17, 0xDD};
    sendCommand(command, sizeof(command));
}

void R200::moduleIdleMode(bool enable) {
    uint8_t command[] = {0xAA, 0x00, 0x04, 0x00, 0x01, enable ? 0x01 : 0x00, 0x00, 0xDD};
    command[6] = calculateChecksum(command, 6);
    sendCommand(command, sizeof(command));
}

bool R200::getResponse(uint8_t *buffer, size_t length, uint32_t timeout) {
    uint32_t startTime = millis();
    size_t bufferIndex = 0;
    int frameLength = -1;
    
    while ((millis() - startTime) < timeout) {
        if (serial.available()) {
            uint8_t byteRead = serial.read();
            if (bufferIndex == 0 && byteRead != 0xAA) {
              continue;
            }

            buffer[bufferIndex++] = byteRead;

            if(bufferIndex >= 5 && frameLength == -1) {
              int payloadLength = (buffer[3] << 8) | buffer[4];
              frameLength = 5 + payloadLength + 2;
            }

            //Serial.print("Byte received: ");
            //Serial.println(byteRead, HEX);

            if(frameLength != -1 && bufferIndex >= frameLength || bufferIndex >= length) {
                return true;
            }
        }
    }
    return false;  // Timeout
}


bool R200::hasValidTag(uint8_t *response) {
    if(response[2] == 0xFF) {
      int res = checkErrorCode(response[5]);
      return false;
    }
   return true;
}


int R200::checkErrorCode(uint8_t code) {
    // returns 0 for ok, 1 for tag nonexistent, 2 for error
    
    switch (code) {
        case 0x17:
            Serial.println("Command Error: Command code error in command frame.");
            return 2;
            break;
        case 0x20:
            Serial.println("FHSS Fail: Frequency hopping search channel timeout, all channels in this section. The time is occupied.");
            return 2;
            break;
        case 0x15:
            //Serial.println("Inventory Fail: Polling operation failed, no label returned or returned CRC check error of return data.");
            return 1;
            break;
        case 0x16:
            Serial.println("Access Fail: Failed to access the tag, possibly the access password. The password is incorrect.");
            return 2;
            break;
        case 0x09:
            Serial.println("Read Fail: Failed to read the tag data storage area. The tag is not returned. Return or return data CRC check error.");
            return 2;
            break;
        case 0xA0:
            Serial.println("Read Error: Error reading label data storage area, return code.");
            return 2;
            break;
        case 0x10:
            Serial.println("Write Fail: Failed to write the tag data storage area. The tag did not return. Return or return data CRC check error.");
            return 2;
            break;
        case 0xB0:
            Serial.println("Write Error: Error writing label data storage area, return code.");
            return 2;
            break;
        case 0x13:
            Serial.println("Lock Fail: Failed to lock label data storage area. No label returned or return data CRC check error.");
            return 2;
            break;
        case 0xC0:
            Serial.println("Lock Error: Error locking label data store, generation returned. The code is obtained from 0xC0 bit or Error Code.");
            return 2;
            break;
        case 0x12:
            Serial.println("Kill Fail: Failed to inactivate the tag. The tag did not return or return CRC check error.");
            return 2;
            break;
        default:
            Serial.print("Unknown Error Code: ");
            Serial.println(code, HEX);
            return 2;
            break;
    }
    return 0;
    
}

void R200::parseTagResponse(uint8_t* response, uint8_t& rssi, uint8_t (&epc)[12]) {
    // Extract RSSI value at position 5
    rssi = response[5];
    
    // Extract EPC starting at position 8 and spanning 12 bytes
    memcpy(epc, &response[8], 12);
}

void R200::sendCommand(uint8_t *command, size_t length) {
    serial.write(command, length);
}

uint8_t R200::calculateChecksum(uint8_t *command, size_t length) {
    uint8_t checksum = 0;
    for (size_t i = 1; i < length; i++) {
        checksum += command[i];
    }
    return checksum & 0xFF;  // Return the least significant byte
}
