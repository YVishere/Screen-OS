# ESP32 OS-Based Prototype with TFT Display, SD Card, and Audio Output

## 📌 Overview
This project is a prototype for building an operating system-like interface on an ESP32, integrating a **TFT display**, **SD card**, and **audio output** using a **MAX98357A DAC**. Key design decisions prioritize stable performance and hardware compatibility.

---

## ⚠️ Notes

- **Power Management**  
  To avoid flickering of the TFT display when audio is active:
  - Use **separate VINs** for the TFT Display and MAX98357A.
  
- **SD Card Compatibility**  
  - SD cards of **32GB or more** often **fail to work** reliably with the `SD.h` library.
  - ✅ **Recommended**: `SanDisk Ultra 16GB SDHC Memory Card, 80MB/s` — tested and working.
 
- **Playing different WAV files**
  - I can only play wav files up to 10 MB reliably.
  - Need to design a protocol to group similar audio files.
  - **FFMPEG command**:  ffmpeg -y -i 505aud.mp3 -ar 22050 -ac 1 -sample_fmt s16 -t 200 5052.wav to bring down the `505` wav file to 10MB.

---

## 📟 Pinout Summary

### 🔹 TFT Display & SD Card (SPI Interface)

| Component       | ESP32 Pin | Description              |
|----------------|-----------|--------------------------|
| TFT/SD Power    | 5V        | VIN for TFT/SD Module (if regulated) |
| MISO           | GPIO 19   | SPI Master In Slave Out  |
| MOSI           | GPIO 23   | SPI Master Out Slave In  |
| SCLK           | GPIO 18   | SPI Clock                |
| DC (TFT)       | GPIO 2    | TFT Data/Command Select  |
| CS (TFT)       | GPIO 15   | TFT Chip Select          |
| CS (SD Card)   | GPIO 2    | SD Card Chip Select      |

> **Note**: Ensure **TFT and SD card do not share the same Chip Select (CS) pin** if used simultaneously. GPIO 2 is reused above — update if needed.

---

### 🔊 Audio Output (MAX98357A DAC)

| Component      | ESP32 Pin | Description         |
|----------------|-----------|---------------------|
| Power          | 3.3V      | VIN for MAX98357A   |
| LRC            | GPIO 14   | Left/Right Clock    |
| BCLK           | GPIO 27   | Bit Clock           |
| DIN            | GPIO 26   | Digital Audio Input |

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
