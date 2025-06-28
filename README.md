# ESP32 OS-Based Prototype with TFT Display, SD Card, and Audio Output

## 📌 Overview
This project is a prototype for building an operating system-like interface on an ESP32, integrating a **TFT display**, **SD card**, and **audio output** using a **MAX98357A DAC**. Key design decisions prioritize stable performance and hardware compatibility.

---

## ⚠️ Notes

- **Power Management**  
 ~~ To avoid flickering of the TFT display when audio is active:~~
  ~~- Use **separate VINs** for the TFT Display and MAX98357A.~~
  The flickering in tft display during audio output was due to me powering the board with through a type-C connection. Using a normal USB port is fine.
  
- **SD Card Compatibility**  
  - SD cards of **32GB or more** often **fail to work** reliably with the `SD.h` library.
  - ✅ **Recommended**: `SanDisk Ultra 16GB SDHC Memory Card, 80MB/s` — tested and working.
 
- **Playing different WAV files**
  - I can only play wav files up to 10 MB reliably.
  - Need to design a protocol to group similar audio files.
  - **FFMPEG command**:  ffmpeg -y -i 505aud.mp3 -ar 22050 -ac 1 -sample_fmt s16 -t 200 5052.wav to bring down the `505` wav file to 10MB.

- **Uploading to flash issue**
  - Cut power to peripherals because the pins being used by the hspi line for SD card are needed to flash the chip. "SPI flash".

---

## 📟 Pinout Summary

### 🔹 TFT Display (VSPI - Default SPI)

| Signal    | ESP32 Pin | Description                |
|-----------|-----------|----------------------------|
| MISO      | GPIO 19   | SPI Master In Slave Out    |
| MOSI      | GPIO 23   | SPI Master Out Slave In    |
| SCLK      | GPIO 18   | SPI Clock                  |
| TFT_CS    | GPIO 15   | TFT Chip Select            |
| TFT_DC    | GPIO 2    | TFT Data/Command Select    |
| TFT_RST   | GPIO 4    | TFT Reset                  |
| Power     | 3.3V      | VIN for TFT                |

### 🔹 SD Card (HSPI - Custom SPI)

| Signal    | ESP32 Pin | Description                |
|-----------|-----------|----------------------------|
| MISO      | GPIO 12   | HSPI Master In Slave Out   |
| MOSI      | GPIO 4    | HSPI Master Out Slave In   |
| SCLK      | GPIO 14   | HSPI Clock                 |
| SD_CS     | GPIO 5    | SD Card Chip Select        |
| Power     | 3.3V      | VIN for SD Card            |

> **Note:**
> - SD card and TFT are now on separate SPI buses (HSPI for SD, VSPI for TFT). This avoids bus contention and allows concurrent use.

---

### 🔊 Audio Output (MAX98357A DAC, I2S)

| Signal      | ESP32 Pin | Description             |
|-------------|-----------|-------------------------|
| Power       | 3.3V      | VIN for MAX98357A       |
| BCLK        | GPIO 27   | I2S Bit Clock           |
| LRC (WS)    | GPIO 25   | I2S Left/Right Clock    |
| DIN         | GPIO 26   | I2S Digital Audio Input |

---

## ✅ To Do / Improvements

- [ ] Optimize display and audio concurrency.
- [ ] Explore FAT32 compatibility for larger SD cards using `SdFat` or `ESP32SPISlave` libraries.
- [ ] Implement low-power modes and voltage regulation monitoring.

---

## 🛠️ Recommended Hardware

- ESP32 DevKit v1
- TFT Display (SPI, 2.4"–2.8")
- MAX98357A I2S DAC
- SanDisk Ultra 16GB SDHC Card
- External 5V Power Supply (for stable VIN separation)

---

## 🔗 License

This prototype is open-source and intended for learning and prototyping use. Feel free to adapt and extend it.
