Prototype to Build on operating system with ESP32, tft display, SD card and speaker.  

Notes  
- Using Seperate VINs for TFT display and MAX98357A is preferred because when the speaker is in USE, the TFT display flickers on same power supply.
- 32GB ot more SD cards are very rarely compatible with SD.h. The working SD card is -- SanDisk Ultra 16GB SDHC Memory Card 80MB/s.

Current PinOuts:

-TFT and SD card related

5V -> VIN for TFT-display/SD card holder (If they have regulators otherwise 3.3V).

GPIO 19 -> MISO

GPIO 23 -> MOSI

GPIO 18 -> SCLK

GPIO 2 -> DC

GPIO 15 -> TFT display Chip Select

GPIO 2 -> SD card Chip Select


-Audio related

3.3V -> VIN for MAX98357A

GPIO 14 -> LRC

GPIO 27 -> BCLK

GPIO 26 -> DIN
