#ifndef __IO_MANAGEMENT_H__
#define __IO_MANAGEMENT_H__

#include <Arduino.h>
#include <Wire.h>

// Macros for pins
#define BUTTON_IO_ADDR  0x27

#define IO_RESET_PIN    17
#define BUTTON_IO_INT   16

#define JOY1_V          33    
#define JOY1_H          25
#define JOY2_V          26
#define JOY2_H          27

#define SD_DET          35

#define JOY2_BUTTON     34
#define SW_OK           36
#define SW_BACK         39

typedef enum{
    BUTTON_PRESSED,
    BUTTON_RELEASED
} ButtonState;

extern volatile bool buttonIoExpanderIntFlag;
extern volatile bool miscIoExpanderIntFlag;

// initialize digital and analog pins, and timer to read pins
void initIO();

void setDir();
uint8_t readButtonIoExpander();
uint8_t readSDStatus();

// ISR to read digital and analog inputs
void IRAM_ATTR buttonIoExpanderISR();

#endif