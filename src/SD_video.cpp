#include "SD_video.h"

int bufferWidth;
int bufferHeight;

int xDisp;
int yDisp;

uint8_t *buffer1;
uint8_t *buffer2;

SemaphoreHandle_t spiMutexBuffer;
SemaphoreHandle_t spiMutexDisp;

char currentFileName[64];

TaskHandle_t loadBuffer1TaskHandle;
TaskHandle_t loadBuffer2TaskHandle;
TaskHandle_t drawBuffer1TaskHandle;
TaskHandle_t drawBuffer2TaskHandle;

int totalFrames = 0;

void countAvailableFrames(const char *FRAME_FILE_PATTERN) {
  totalFrames = 0;
  char currentFramePath[64];
  while(true) {
    sprintf(currentFramePath, FRAME_FILE_PATTERN, totalFrames);
    if(!SD.exists(currentFramePath)) {
      break;
    }
    totalFrames++;
  }
  Serial.printf("Found %d animation frames\n", totalFrames);
}

void loadBuffer1(void *pvParameters) {
    // Load the first buffer with video data
    int frameIndex = 0;
    bool active = false;

    const char *FRAME_FILE_PATTERN = (const char *)pvParameters;
    File vidFile;

    while(true){
        if (active){
            if (frameIndex >= totalFrames){
                frameIndex -= totalFrames; // Loop back to the start
            }
            sprintf(currentFileName, FRAME_FILE_PATTERN, frameIndex);

            xSemaphoreTake(spiMutexBuffer, portMAX_DELAY);

            vidFile = SD.open(currentFileName, FILE_READ);
            if (!vidFile) {
                Serial.printf("Failed to open file %s\n", currentFileName);
                break;
            }
            
            int bytesRead = vidFile.read(buffer1, bufferWidth * bufferHeight);
            vidFile.close();
            
            Serial.printf("Loading buffer 1 for file %s\n", currentFileName);

            xSemaphoreGive(spiMutexBuffer);
            xTaskNotifyGive(drawBuffer1TaskHandle); // Notify the draw task to start drawing this buffer
            frameIndex += 2;
        }
        else{
            // Wait for the task to be notified to start loading
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            active = true;
            Serial.println("Buffer 1 loading started");
        }
    }

    vTaskDelete(NULL);
}

void loadBuffer2(void *pvParameters) {
    // Load the second buffer with video data
    // Load the first buffer with video data
    int frameIndex = 1;
    bool active = false;

    const char *FRAME_FILE_PATTERN = (const char *)pvParameters;
    File vidFile;

    while(true){
        if(active){
            if (frameIndex >= totalFrames){
                frameIndex -= totalFrames; // Loop back to the start
            }
            sprintf(currentFileName, FRAME_FILE_PATTERN, frameIndex);

            xSemaphoreTake(spiMutexBuffer, portMAX_DELAY);

            vidFile = SD.open(currentFileName, FILE_READ);
            if (!vidFile) {
                Serial.printf("Failed to open file %s\n", currentFileName);
                break;
            }
            
            int bytesRead = vidFile.read(buffer2, bufferWidth * bufferHeight);
            vidFile.close();
            
            Serial.printf("Loading buffer 2 for file %s\n", currentFileName);

            xSemaphoreGive(spiMutexBuffer);
            xTaskNotifyGive(drawBuffer2TaskHandle); // Notify the draw task to start drawing this buffer
            frameIndex += 2;
        }
        else{
            // Wait for the task to be notified to start loading
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            active = true;
            Serial.println("Buffer 2 loading started");
        }
    }

    vTaskDelete(NULL);
}

void drawBuffer1(void *pvParameters) {
    // Draw the first buffer to the display
    bool active = false;

    while(true){
        if (active){
            xSemaphoreTake(spiMutexDisp, portMAX_DELAY);
            tft.pushImage(xDisp, yDisp, bufferWidth, bufferHeight, buffer1);
            xSemaphoreGive(spiMutexDisp);
            xTaskNotifyGive(loadBuffer1TaskHandle); // Notify the load task to start loading the next frame
            Serial.println("Drawing buffer 1 to display");
        }
        else{
            // Wait for the task to be notified to start drawing
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            active = true;
            Serial.println("Buffer 1 drawing started");
        }
    }
    vTaskDelete(NULL);
}

void drawBuffer2(void *pvParameters) {
    // Draw the second buffer to the display
    bool active = false;
    while(true){
        if (active){
            xSemaphoreTake(spiMutexDisp, portMAX_DELAY);
            tft.pushImage(xDisp, yDisp, bufferWidth, bufferHeight, buffer2);
            xSemaphoreGive(spiMutexDisp);
            xTaskNotifyGive(loadBuffer2TaskHandle); // Notify the load task to start loading the next frame
            Serial.println("Drawing buffer 2 to display");
        }
        else{
            // Wait for the task to be notified to start drawing
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            active = true;
            Serial.println("Buffer 2 drawing started");
        }
    }
    vTaskDelete(NULL);
}

void startSDVideo(const char *file_name, int x, int y, int width, int height){

    bufferWidth = width;
    bufferHeight = height;

    xDisp = x;
    yDisp = y;

    buffer1 = (uint8_t *)heap_caps_malloc(width*height*sizeof(uint8_t), MALLOC_CAP_DMA);
    buffer2 = (uint8_t *)heap_caps_malloc(width*height*sizeof(uint8_t), MALLOC_CAP_DMA);
    if (!buffer1 || !buffer2) {
        Serial.println("Failed to allocate memory for video buffers");
        return;
    }

    spiMutexBuffer = xSemaphoreCreateMutex();
    spiMutexDisp = xSemaphoreCreateMutex();
    if (!spiMutexBuffer || !spiMutexDisp) {
        Serial.println("Failed to create semaphores for video buffers");
        return;
    }

    xTaskCreatePinnedToCore(
        loadBuffer1,           // Task function
        "LoadBuffer1",         // Name of task
        4096,                  // Stack size in words
        (void *) file_name,                  // Task input parameter
        2,                     // Priority of the task
        &loadBuffer1TaskHandle,// Task handle
        1                      // Core ID (e.g., 0 or 1)
    );

    xTaskCreatePinnedToCore(
        loadBuffer2,           // Task function
        "LoadBuffer2",         // Name of task
        4096,                  // Stack size in words
        (void *) file_name,                  // Task input parameter
        2,                     // Priority of the task
        &loadBuffer2TaskHandle,// Task handle
        1                      // Core ID (e.g., 0 or 1)
    );

    xTaskCreatePinnedToCore(
        drawBuffer1,           // Task function
        "DrawBuffer1",         // Name of task
        4096,                  // Stack size in words
        NULL,                  // Task input parameter
        2,                     // Priority of the task
        &drawBuffer1TaskHandle,// Task handle
        0                      // Core ID (e.g., 0 or 1)
    );

    xTaskCreatePinnedToCore(
        drawBuffer2,           // Task function
        "DrawBuffer2",         // Name of task
        4096,                  // Stack size in words
        NULL,                  // Task input parameter
        2,                     // Priority of the task
        &drawBuffer2TaskHandle,// Task handle
        0                      // Core ID (e.g., 0 or 1)
    );

    xTaskNotifyGive(loadBuffer1TaskHandle);
    xTaskNotifyGive(loadBuffer2TaskHandle);
    xSemaphoreGive(spiMutexBuffer);
}