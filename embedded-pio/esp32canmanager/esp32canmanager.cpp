#include "esp32canmanager.h"

ESP32CANManager::ESP32CANManager(int8_t tx, int8_t rx, uint16_t tx_queue, uint16_t rx_queue, uint16_t frequency) {
    if (ESP32Can.begin(ESP32Can.convertSpeed(frequency), tx, rx, tx_queue, rx_queue)) {
        printf("can bus started\n");
    } else {
        printf("can bus initialization FAIL\n");
    }
}

bool ESP32CANManager::sendMessage(uint16_t id, void* data, uint8_t length, uint32_t timeout) {
    // create the message
    CanFrame obdFrame = { 0 };
	obdFrame.identifier = id;
	obdFrame.extd = 0; // use non extended 11 bit ID
	obdFrame.data_length_code = length;
    memcpy(obdFrame.data, data, length);
    // printf("obdFrame: %x\n", obdFrame.data[0]);

    uint32_t start = millis();
    bool send_success = false;
    while (!(send_success = ESP32Can.writeFrame(obdFrame)) && millis() - start < timeout){
        this->runQueue(1);
    }

    return send_success;  
}

void ESP32CANManager::runQueue(uint16_t duration) {
    CanFrame rxFrame;

    uint32_t start = millis();
    
    while (millis() - start < duration) {
        if(ESP32Can.readFrame(rxFrame, 1)) {
            this->readHandler(rxFrame);
        }
    }
}