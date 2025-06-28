#ifndef __SD_VIDEO_H
#define __SD_VIDEO_H

#include <Arduino.h>
#include "TFT_eSPI.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include <SPI.h>
#include <SD.h>
#include <FS.h>

extern TFT_eSPI tft;

void startSDVideo(const char *file_name, int x, int y, int width, int height);
void countAvailableFrames(const char *FRAME_FILE_PATTERN);

void initializeWatchdog();
void addTaskToWatchdog(TaskHandle_t taskHandle, const char* taskName);
void feedWatchdog();
int getNextFrameIndex();

void loadBuffer1(void *pvParameters);
void loadBuffer2(void *pvParameters);

void drawBuffer1(void *pvParameters);
void drawBuffer2(void *pvParameters);

#endif