#include <Arduino.h>
#include <SD.h>
#include "WAVFileReader.h"
#include "I2SOutput.h"
#include "AVIFileReader.h"
#include "TFT_output.h"
#include "display.h"
#include "SD_video.h"

i2s_pin_config_t i2sPins = {
    .bck_io_num = GPIO_NUM_27,
    .ws_io_num = GPIO_NUM_25,
    .data_out_num = GPIO_NUM_26,
    .data_in_num = -1};

I2SOutput *output;
SampleSource *sampleSource;

// Video playback components (similar to audio)
TFT_Output *videoOutput;
FrameSource *videoSource;
extern TFT_eSPI tft; // Declared in display.cpp

const char *FRAME_FILE_PATTERN = "/output_frame/frame%d.bin";

// Function to demonstrate AVIFileReader and TFT_Output usage
void setupVideoPlayback() {
  // Example setup for video playback - similar to audio setup
  if (SD.exists("/video.avi")) {
    Serial.println("Found video.avi file, setting up video playback...");
    
    // Create video source (AVI file reader)
    videoSource = new AVIFileReader("/video.avi");
    
    // Create video output (TFT display)
    videoOutput = new TFT_Output();
    
    // Start video playback on TFT display
    // Parameters: TFT instance, video source, x, y, width, height
    videoOutput->start(&tft, videoSource, 0, 0, 160, 128);
    
    Serial.println("Video playback started");
  } else {
    Serial.println("No video.avi file found, skipping video playback setup");
  }
}

// Function to demonstrate WAVFileReader and I2SOutput usage  
void setupAudioPlayback() {
  // Example setup for audio playback
  if (SD.exists("/5052.wav")) {
    Serial.println("Found audio file, setting up audio playback...");
    
    // Create audio source (WAV file reader)
    sampleSource = new WAVFileReader("/5052.wav");
    
    // Create audio output (I2S)
    output = new I2SOutput();
    
    // Start audio playback on I2S
    output->start(I2S_NUM_1, i2sPins, sampleSource);
    
    Serial.println("Audio playback started");
  } else {
    Serial.println("No audio file found, skipping audio playback setup");
  }
}

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

  // Uncomment one of these functions to enable playback:
  
  // For video playback using AVI files:
  // setupVideoPlayback();
  
  // For audio playback using WAV files:
  // setupAudioPlayback();

  // Legacy examples (currently commented out)
  // Audio playback example (currently commented out)
  // sampleSource = new WAVFileReader("/5052.wav");
  // Serial.println("Starting I2S Output");
  // output = new I2SOutput();
  // output->start(I2S_NUM_1, i2sPins, sampleSource);

  // Video playback example using AVIFileReader and TFT_Output
  // Uncomment the lines below to use AVI file playback instead of frame files
  // videoSource = new AVIFileReader("/video.avi");
  // Serial.println("Starting TFT Video Output");
  // videoOutput = new TFT_Output();
  // videoOutput->start(&tft, videoSource, 0, 0, 160, 128);
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