#include "display.h"

TFT_eSPI tft = TFT_eSPI();

//Array of random colors to cycle through
uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};

void rotateColors() {
  static uint8_t colorIndex = 0;
  tft.fillScreen(colors[colorIndex]);
  colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
}

void initDisplay(bool SD_enable){
    // Set all chip selects high to avoid bus contention during initialisation of each peripheral
    digitalWrite(22, HIGH); // Touch controller chip select (if used)
    digitalWrite(15, HIGH); // TFT screen chip select
    digitalWrite( 5, HIGH); // SD card chips select, must use GPIO 5 (ESP32 SS)

    tft.begin();
    tft.setRotation(1);  // Somehow important to have an odd rotation number
                          //Display does not work properly if you do not set this and initialise the SD card
    tft.fillScreen(TFT_CYAN);

    if (!SD_enable) {
      Serial.println("Display initialized without SD card.");
      return;
    }

    if (!SD.begin(5, tft.getSPIinstance())) {
      Serial.println("Card Mount Failed");
      return;
    }
    uint8_t cardType = SD.cardType();
  
    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
    }
  
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
      Serial.println("MMC");
    } else if (cardType == CARD_SD) {
      Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
    }

    // // Check if frame files need to be created from text
    // bool needsConversion = false;
    // for (int i = 0; i < 14; i++) {
    //   sprintf(currentFramePath, FRAME_FILE_PATTERN, i);
    //   if (!SD.exists(currentFramePath)) {
    //     needsConversion = true;
    //     break;
    //   }
    // }

    // // Convert the text animation to individual binary frame files if needed
    // if (needsConversion && SD.exists("/bsr/som.txt")) {
    //   Serial.println("Creating individual binary frame files...");
    //   convertTextToBinaryFrames("/bsr/som.txt");
    // }
  
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
  
    // countAvailableFrames();
    // Serial.printf("Total frames available: %d\n", totalFrames);

    Serial.println("Initialization done.");
}