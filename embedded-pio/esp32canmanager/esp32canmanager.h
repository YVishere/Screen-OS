#ifndef __ESP32_CAN_H__
#define __ESP32_CAN_H__

#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

#define DEFAULT_ESP32_CAN_FREQ 250

class ESP32CANManager {
    public:
        /* frequency: in kHz 
         * tx: pin number
         * rx: pin number
         * tx_queue: number of messages in TX queue
         * rx_queue: number of messages in RX queue
         */
        ESP32CANManager(int8_t tx, int8_t rx, uint16_t tx_queue, uint16_t rx_queue, uint16_t frequency = DEFAULT_ESP32_CAN_FREQ);

        /* Handles data from received messages
         * Intended to be implemented by class extension per board
         * Called in runQueue()
         */
        virtual void readHandler(CanFrame msg) = 0;

        /* Send a message over CAN
         *
         * id: CAN ID to use to identify the signal
         * data: Payload array
         * length: Size of data in bytes
         * timeout: in milliseconds
         * 
         * @return true if message sent successfully, false otherwise
         */ 
        bool sendMessage(uint16_t id, void* data, uint8_t length, uint32_t timeout = 1);


        /* Processes CAN (read) messages stored in messageQueue for a set duration. 
         * THIS IS THE FUNCTION TO CALL FOR PROCESSING CAN READ MESSAGES
         * 
         * duration: time in milliseconds
         */
        void runQueue(uint16_t duration);
};

#endif