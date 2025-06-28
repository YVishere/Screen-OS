#include "display.h"

void sanity_check();

SPIClass hspi(HSPI);

TFT_eSPI tft = TFT_eSPI();

uint16_t colors[] = {
    TFT_BLACK, TFT_NAVY, TFT_DARKGREEN, TFT_DARKCYAN, 
    TFT_MAROON, TFT_PURPLE, TFT_OLIVE, TFT_LIGHTGREY,
    TFT_DARKGREY, TFT_BLUE, TFT_GREEN, TFT_CYAN,
    TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_WHITE
};

void rotateColors(){
  static int colorIndex = 0;
  tft.fillScreen(colors[colorIndex]);
  colorIndex++;
}

void initDisplay(){
    pinMode(5, OUTPUT);
    pinMode(15, OUTPUT);
    digitalWrite(5, HIGH); 
    digitalWrite(15, HIGH); 

    tft.begin();
    tft.setRotation(1);

    tft.fillScreen(TFT_CYAN);

    hspi.begin(14, 12, 4, 5); // SCK, MISO, MOSI, CS

    if (!SD.begin(5, hspi)){
        Serial.printf("SD card failed");
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

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.println("Initialization done.");

    File root = SD.open("/");
    Serial.printf("Printing SD file system\n");
    Serial.printf("###############################################\n");
    printDirectory(root, 0);
    Serial.printf("###############################################\n");

    Serial.println("Sanity check:");
    sanity_check();
}

//longtext anothtext sanitycheck
void sanity_check(){
  uint8_t *buffer = (uint8_t *)malloc(100*sizeof(uint8_t));
  if(!SD.exists("/anothtext.txt")){
    Serial.println("Sanity check fail");
    return;
  }
  File sanityFile = SD.open("/anothtext.txt", FILE_READ);
  if (!sanityFile) {
    Serial.println("Failed to open sanity check file");
    return;
  }
  Serial.println("Sanity check file opened successfully");

  // while (sanityFile.available()) {
  //   Serial.print((char)sanityFile.read());
  // }
  size_t bytesRead = sanityFile.read(buffer, 44);
  for (size_t i = 0; i < bytesRead; i++) {
    Serial.printf("%c", buffer[i]);
  }
  Serial.println("\nSanity check file read successfully");
  Serial.printf("Bytes read: %d\n", bytesRead);
  sanityFile.close();


  ///////////////////////////////////////////////////////////
  free(buffer);
  buffer = (uint8_t *)malloc(100*sizeof(uint8_t));
  if(!SD.exists("/sanitycheck.txt")){
    Serial.println("Sanity check fail");
    return;
  }
  sanityFile = SD.open("/sanitycheck.txt", FILE_READ);
  if (!sanityFile) {
    Serial.println("Failed to open sanity check file");
    return;
  }
  Serial.println("Sanity check file opened successfully");

  // while (sanityFile.available()) {
  //   Serial.print((char)sanityFile.read());
  // }
  bytesRead = sanityFile.read(buffer, 44);
  for (size_t i = 0; i < bytesRead; i++) {
    Serial.printf("%c", buffer[i]);
  }
  Serial.println("\nSanity check file read successfully");
  Serial.printf("Bytes read: %d\n", bytesRead);
  sanityFile.close();
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break; // No more files in this directory
    }
    // Skip the output_frames directory
    if (entry.isDirectory() && String(entry.name()) == "output_frame") {
      entry.close();
      continue;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      if (i == 0) {
        Serial.print("|");
      }
      Serial.print("-\t"); // Add tabs for indentation
    }
    Serial.print(entry.name()); // Print the file name
    if (entry.isDirectory()) {
      Serial.println("/"); // Indicate it's a directory
      printDirectory(entry, numTabs + 1); // Recursively print subdirectories
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC); // Print the file size
    }
    entry.close();
  }
}