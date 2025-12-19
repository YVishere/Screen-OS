#include <Arduino.h>
#include "IOManagement.h"
#include "display.h"
#include "SD_intf.h"

#include "debug.h"

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(JOY1_H, INPUT);
    pinMode(JOY1_V, INPUT);
    pinMode(JOY2_H, INPUT);
    pinMode(JOY2_V, INPUT);
    
    initIO();

    if (readSDStatus() == 1) {
        Serial.println("SD Card in MMC holder Detected");
        if (init_sdmmc_idf(false) != ESP_OK) {
            printf("Retry 1-bit...\n");
            init_sdmmc_idf(true);
        }
        initDisplay(false);
    }
    else {
        initDisplay(true);
    }
}

void loop() {
    #if DEBUG_LOOP == 0
        // Main loop code here
    #else
        static int joh1h = 0;
        static int joh2h = 0;
        static int joy1v = 0;
        static int joy2v = 0;
        static uint8_t buttonStates = 0;
        static uint8_t miscStates = 0;
        static ButtonState prevOK = BUTTON_RELEASED;
        static ButtonState prevBACK = BUTTON_RELEASED;
        static ButtonState prevJOY2 = BUTTON_RELEASED;

        static int sd_detected = 0;

        rotateColors();


        if (buttonIoExpanderIntFlag) {
            buttonStates = readButtonIoExpander();
        }

        int new_joh1h = analogRead(JOY1_H);
        int new_joy1v = analogRead(JOY1_V);
        int new_joh2h = analogRead(JOY2_H);
        int new_joy2v = analogRead(JOY2_V);
        if (abs(new_joh1h - joh1h) > 100 || abs(new_joy1v - joy1v) > 100 || 
            abs(new_joh2h - joh2h) > 100 || abs(new_joy2v - joy2v) > 100) {
            joh1h = new_joh1h;
            joy1v = new_joy1v;
            joh2h = new_joh2h;
            joy2v = new_joy2v;

            Serial.print("Joy1 H: ");
            Serial.print(joh1h);
            Serial.print(" V: ");
            Serial.print(joy1v);
            Serial.print(" | Joy2 H: ");
            Serial.print(joh2h);
            Serial.print(" V: ");
            Serial.print(joy2v);
            Serial.print(" | Buttons: ");
            Serial.print(buttonStates, BIN);
            Serial.println();
        }

        ButtonState currJOY2 = (digitalRead(JOY2_BUTTON) == LOW) ? BUTTON_PRESSED : BUTTON_RELEASED;
        ButtonState currOK = (digitalRead(SW_OK) == LOW) ? BUTTON_PRESSED : BUTTON_RELEASED;
        ButtonState currBACK = (digitalRead(SW_BACK) == LOW) ? BUTTON_PRESSED : BUTTON_RELEASED;
        
        if (currJOY2 != prevJOY2) {
            prevJOY2 = currJOY2;
            Serial.print("JOY2 Button: ");
            Serial.println((currJOY2 == BUTTON_PRESSED) ? "PRESSED" : "RELEASED");
        }

        if (currOK != prevOK) {
            prevOK = currOK;
            Serial.print("SW_OK Button: ");
            Serial.println((currOK == BUTTON_PRESSED) ? "PRESSED" : "RELEASED");
        }

        if (currBACK != prevBACK) {
            prevBACK = currBACK;
            Serial.print("SW_BACK Button: ");
            Serial.println((currBACK == BUTTON_PRESSED) ? "PRESSED" : "RELEASED");
        }

        prevJOY2 = currJOY2;
        prevOK = currOK;
        prevBACK = currBACK;

        // sd_detected = digitalRead(SD_DET);
        // if (sd_detected == HIGH) {
        //     Serial.println("SD Detected: Yes");
        // } else {
        //     Serial.println("SD Detected: No");
        // }
    #endif
}
