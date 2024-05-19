#ifndef RFIDR200_H
#define RFIDR200_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class RFIDR200 {
public:
    // Constructors for HardwareSerial and SoftwareSerial
    RFIDR200(HardwareSerial &serial, uint32_t baudRate);
    RFIDR200(SoftwareSerial &serial, uint32_t baudRate);

    void begin();
    void setTransmitPower(uint16_t power);
    void readTagData(uint32_t accessPassword, uint8_t memBank, uint16_t address, uint16_t length, uint8_t *data);
    void writeTagData(uint32_t accessPassword, uint8_t memBank, uint16_t address, uint16_t length, uint8_t *data);
    void initiateSinglePolling();
    void initiateMultiplePolling(uint16_t count);
    void stopMultiplePolling();
    void setSelectParameter(uint8_t target, uint8_t action, uint8_t memBank, uint32_t pointer, uint8_t maskLength, uint8_t *mask);
    void getSelectParameter(uint8_t *response, size_t length);
    void killTag(uint32_t killPassword);
    void lockTag(uint32_t accessPassword, uint16_t lockPayload);
    void setWorkArea(uint8_t region);
    void getWorkArea(uint8_t *response, size_t length);
    void setWorkingChannel(uint8_t channelIndex);
    void getWorkingChannel(uint8_t *response, size_t length);
    void setAutomaticFrequencyHopping(bool enable);
    void insertWorkingChannel(uint8_t count, uint8_t *channelList);
    void moduleSleep();
    void moduleIdleMode(bool enable);
    bool getResponse(uint8_t *buffer, size_t length, uint32_t timeout = 1000);
    bool hasValidTag(uint8_t *response);
    int checkErrorCode(uint8_t code);
    void parseTagResponse(uint8_t* response, uint8_t& rssi, uint8_t (&epc)[12]);


    

private:
    Stream &serial;
    uint32_t baudRate;
    bool isHardwareSerial;

    void sendCommand(uint8_t *command, size_t length);
    uint8_t calculateChecksum(uint8_t *command, size_t length);
};

#endif // RFIDR200_H
