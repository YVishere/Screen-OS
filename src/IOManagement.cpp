#include "IOManagement.h"

volatile bool buttonIoExpanderIntFlag = false;

// MCP23009 Register Addresses
#define IODIR_REG   0x00  // I/O Direction register
#define IPOL_REG    0x01  // Input polarity register
#define GPINTEN_REG 0x02  // Interrupt-on-change enable
#define DEFVAL_REG  0x03  // Default compare value
#define INTCON_REG  0x04  // Interrupt control
#define IOCON_REG   0x05  // Configuration register
#define GPPU_REG    0x06  // Pull-up resistor config
#define INTF_REG    0x07  // Interrupt flag register
#define INTCAP_REG  0x08  // Interrupt capture register
#define GPIO_IO_REG    0x09  // GPIO port register
#define OLAT_REG    0x0A  // Output latch register

void initIO() {
    Wire.begin();
    
    Serial.println("\nI2C Scanner");
    Serial.println("Scanning...");
    
    byte count = 0;
    
    for (byte address = 0; address <= 127; address++) {
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println();
            count++;
        }
    }
    
    Serial.println("\nScan complete.");
    Serial.print("Total devices found: ");
    Serial.println(count);
    
    pinMode(JOY1_H, INPUT);
    pinMode(JOY1_V, INPUT);
    pinMode(JOY2_H, INPUT);
    pinMode(JOY2_V, INPUT);

    pinMode(IO_RESET_PIN, OUTPUT);
    digitalWrite(IO_RESET_PIN, LOW); // Assert reset
    delay(10);
    digitalWrite(IO_RESET_PIN, HIGH); // Deassert reset

    pinMode(BUTTON_IO_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_IO_INT), buttonIoExpanderISR, FALLING);

    pinMode(SD_DET, INPUT);

    pinMode(JOY1_BUTTON, INPUT);
    pinMode(JOY2_BUTTON, INPUT);
    pinMode(SW_OK, INPUT);
    pinMode(SW_BACK, INPUT);

    setDir();
}

void setDir(){
    // Configure Button IO Expander
    Wire.beginTransmission(BUTTON_IO_ADDR);
    Wire.write(IODIR_REG);
    Wire.write(0xFF); // Set all pins as inputs
    Wire.endTransmission();

    // Enable pull-ups for button inputs
    Wire.beginTransmission(BUTTON_IO_ADDR);
    Wire.write(GPPU_REG);
    Wire.write(0xFF); // Enable pull-ups on all pins
    Wire.endTransmission();

    // Enable interrupts on all pins (interrupt-on-change)
    Wire.beginTransmission(BUTTON_IO_ADDR);
    Wire.write(GPINTEN_REG);
    Wire.write(0xFF); // Enable interrupt on all pins
    Wire.endTransmission();

    // Configure interrupt to trigger on change from previous value
    Wire.beginTransmission(BUTTON_IO_ADDR);
    Wire.write(INTCON_REG);
    Wire.write(0x00); // Compare to previous pin value (not DEFVAL)
    Wire.endTransmission();
}

uint8_t readButtonIoExpander() {
    Serial.println("Button IO Expander Interrupt Triggered");

    uint8_t buttonStates = 0xFF; // Default all released

    // Read INTCAP to clear the interrupt and get captured values
    Wire.beginTransmission(BUTTON_IO_ADDR);
    Wire.write(INTCAP_REG);
    Wire.endTransmission();

    Wire.requestFrom(BUTTON_IO_ADDR, 1);
    if (Wire.available()) {
        buttonStates = Wire.read();
        Serial.print("Button States (from INTCAP): ");
        Serial.println(buttonStates, BIN);
    }

    // Optionally read GPIO for current state
    Wire.beginTransmission(BUTTON_IO_ADDR);
    Wire.write(GPIO_IO_REG);
    Wire.endTransmission();

    Wire.requestFrom(BUTTON_IO_ADDR, 1);
    if (Wire.available()) {
        buttonStates = Wire.read();
        Serial.print("Button States (current GPIO): ");
        Serial.println(buttonStates, BIN);
    }

    // Clear the interrupt flag after handling
    buttonIoExpanderIntFlag = false;
    return buttonStates;
}

uint8_t readSDStatus() {
    return digitalRead(SD_DET);
} 

void IRAM_ATTR buttonIoExpanderISR() {
    buttonIoExpanderIntFlag = true;
}