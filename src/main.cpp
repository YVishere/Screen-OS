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

const char *FRAME_FILE_PATTERN = "output_frame/frame%04d.bin";

void setup() {
  Serial.begin(115200);
  delay(1000); 

  initDisplay();

  Serial.println("Starting VID");

  startSDVideo(FRAME_FILE_PATTERN, 0, 0, 128, 160);

  // sampleSource = new WAVFileReader("/5052.wav");

  // Serial.println("Starting I2S Output");
  // output = new I2SOutput();
  // output->start(I2S_NUM_1, i2sPins, sampleSource);
}

void loop() {
  // put your main code here, to run repeatedly:
}