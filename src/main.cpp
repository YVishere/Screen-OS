#include <Arduino.h>
#include <SD.h>
#include "WAVFileReader.h"
#include "I2SOutput.h"
#include "display.h"
#include "SD_video.h"

i2s_pin_config_t i2sPins = {
    .bck_io_num = GPIO_NUM_27,
    .ws_io_num = GPIO_NUM_25,
    .data_out_num = GPIO_NUM_26,
    .data_in_num = -1};

I2SOutput *output;
SampleSource *sampleSource;

const char *FRAME_FILE_PATTERN = "/output_frame/frame%d.bin";

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println("=== ESP32 Video Player Starting ===");
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("PSRAM available: %s\n", psramFound() ? "Yes" : "No");

  Serial.println("Initializing display and SD card...");
  initDisplay();

  // Add delay to ensure SD card is fully initialized
  delay(500);

  Serial.println("Starting VID");
  startSDVideo(FRAME_FILE_PATTERN, 0, 0, 160, 128);

  Serial.printf("Setup complete. Free heap: %d bytes\n", ESP.getFreeHeap());

  // sampleSource = new WAVFileReader("/5052.wav");

  // Serial.println("Starting I2S Output");
  // output = new I2SOutput();
  // output->start(I2S_NUM_1, i2sPins, sampleSource);
}

void loop() {
  // Add watchdog feeding in main loop and memory monitoring
  static unsigned long lastHeapCheck = 0;
  if (millis() - lastHeapCheck > 5000) { // Every 5 seconds
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    lastHeapCheck = millis();
  }
  delay(100);
}