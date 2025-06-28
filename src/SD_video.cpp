#include "SD_video.h"

// Global variables for watchdog management
bool watchdogInitialized = false;

void initializeWatchdog() {
    if (!watchdogInitialized) {
        // Configure watchdog timeout to 10 seconds
        esp_task_wdt_init(10, true);
        watchdogInitialized = true;
        Serial.println("Watchdog initialized with 10 second timeout");
    }
}

void addTaskToWatchdog(TaskHandle_t taskHandle, const char* taskName) {
    if (watchdogInitialized && taskHandle != NULL) {
        esp_task_wdt_add(taskHandle);
        Serial.printf("Added task %s to watchdog\n", taskName);
    }
}

void feedWatchdog() {
    if (watchdogInitialized) {
        esp_task_wdt_reset();
    }
}

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

// Global frame management
volatile int currentFrameIndex = 1;
SemaphoreHandle_t frameIndexMutex;

int getNextFrameIndex() {
    int frameIndex;
    if (xSemaphoreTake(frameIndexMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        frameIndex = currentFrameIndex;
        currentFrameIndex++;
        if (currentFrameIndex > totalFrames) {
            currentFrameIndex = 1; // Loop back to start
        }
        xSemaphoreGive(frameIndexMutex);
    } else {
        frameIndex = 1; // fallback
    }
    return frameIndex;
}

void countAvailableFrames(const char *FRAME_FILE_PATTERN) {
  totalFrames = 0;
  char currentFramePath[64];
  Serial.println("Counting available frames...");
  
  // Start from frame1 (since your logs show frame2.bin as the first frame)
  for(int i = 1; i < 1000; i++) { // reasonable upper limit
    sprintf(currentFramePath, FRAME_FILE_PATTERN, i);
    if(!SD.exists(currentFramePath)) {
      break;
    }
    totalFrames++;
  }
  Serial.printf("Found %d animation frames\n", totalFrames);
}

void loadBuffer1(void *pvParameters) {
    // Load the first buffer with video data
    bool active = false;

    const char *FRAME_FILE_PATTERN = (const char *)pvParameters;
    File vidFile;

    // Add this task to watchdog
    addTaskToWatchdog(xTaskGetCurrentTaskHandle(), "LoadBuffer1");

    while(true){
        // Feed watchdog at the beginning of each loop
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(1));
        
        if (active){
            int frameIndex = getNextFrameIndex();
            sprintf(currentFileName, FRAME_FILE_PATTERN, frameIndex);

            // Use timeout for semaphore to prevent deadlock
            if (xSemaphoreTake(spiMutexBuffer, pdMS_TO_TICKS(1000)) == pdTRUE) {
                vidFile = SD.open(currentFileName, FILE_READ);
                if (!vidFile) {
                    Serial.printf("Failed to open file %s\n", currentFileName);
                    xSemaphoreGive(spiMutexBuffer);
                    vTaskDelay(pdMS_TO_TICKS(100)); // Wait before retrying
                    continue;
                }
                
                int bytesRead = vidFile.read(buffer1, bufferWidth * bufferHeight);
                vidFile.close();
                
                Serial.printf("Loading buffer 1 for file %s\n", currentFileName);

                xSemaphoreGive(spiMutexBuffer);
                
                // Wait for draw task to finish before notifying
                xTaskNotifyGive(drawBuffer1TaskHandle); // Notify the draw task to start drawing this buffer
            } else {
                Serial.println("Buffer1: Failed to acquire semaphore");
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }
        }
        else{
            // Wait for the task to be notified to start loading with timeout
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) > 0) {
                active = true;
                Serial.println("Buffer 1 loading started");
            }
        }
    }

    vTaskDelete(NULL);
}

void loadBuffer2(void *pvParameters) {
    // Load the second buffer with video data
    bool active = false;

    const char *FRAME_FILE_PATTERN = (const char *)pvParameters;
    File vidFile;

    // Add this task to watchdog
    addTaskToWatchdog(xTaskGetCurrentTaskHandle(), "LoadBuffer2");

    while(true){
        // Feed watchdog at the beginning of each loop
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(1));
        
        if(active){
            int frameIndex = getNextFrameIndex();
            sprintf(currentFileName, FRAME_FILE_PATTERN, frameIndex);

            // Use timeout for semaphore to prevent deadlock
            if (xSemaphoreTake(spiMutexBuffer, pdMS_TO_TICKS(1000)) == pdTRUE) {
                vidFile = SD.open(currentFileName, FILE_READ);
                if (!vidFile) {
                    Serial.printf("Failed to open file %s\n", currentFileName);
                    xSemaphoreGive(spiMutexBuffer);
                    vTaskDelay(pdMS_TO_TICKS(100)); // Wait before retrying
                    continue;
                }
                
                int bytesRead = vidFile.read(buffer2, bufferWidth * bufferHeight);
                vidFile.close();
                
                Serial.printf("Loading buffer 2 for file %s\n", currentFileName);

                xSemaphoreGive(spiMutexBuffer);
                
                // Wait for draw task to finish before notifying
                xTaskNotifyGive(drawBuffer2TaskHandle); // Notify the draw task to start drawing this buffer
            } else {
                Serial.println("Buffer2: Failed to acquire semaphore");
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }
        }
        else{
            // Wait for the task to be notified to start loading with timeout
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) > 0) {
                active = true;
                Serial.println("Buffer 2 loading started");
            }
        }
    }

    vTaskDelete(NULL);
}

void drawBuffer1(void *pvParameters) {
    // Draw the first buffer to the display
    bool active = false;

    // Add this task to watchdog
    addTaskToWatchdog(xTaskGetCurrentTaskHandle(), "DrawBuffer1");

    while(true){
        // Feed watchdog at the beginning of each loop
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(1));
        
        if (active){
            // Use timeout for semaphore to prevent deadlock
            if (xSemaphoreTake(spiMutexDisp, pdMS_TO_TICKS(1000)) == pdTRUE) {
                tft.pushImage(xDisp, yDisp, bufferWidth, bufferHeight, buffer1);
                xSemaphoreGive(spiMutexDisp);
                Serial.println("Drawing buffer 1 to display");
                
                // Add frame rate control delay
                vTaskDelay(pdMS_TO_TICKS(66)); // ~15 FPS (more stable)
                
                // Trigger next load after drawing and delay
                xTaskNotifyGive(loadBuffer1TaskHandle); // Notify the load task to start loading the next frame
                active = false; // Reset state to wait for next notification
            } else {
                Serial.println("DrawBuffer1: Failed to acquire display semaphore");
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        else{
            // Wait for the task to be notified to start drawing with timeout
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) > 0) {
                active = true;
                Serial.println("Buffer 1 drawing started");
            }
        }
    }
    vTaskDelete(NULL);
}

void drawBuffer2(void *pvParameters) {
    // Draw the second buffer to the display
    bool active = false;
    
    // Add this task to watchdog
    addTaskToWatchdog(xTaskGetCurrentTaskHandle(), "DrawBuffer2");
    
    while(true){
        // Feed watchdog at the beginning of each loop
        feedWatchdog();
        vTaskDelay(pdMS_TO_TICKS(1));
        
        if (active){
            // Use timeout for semaphore to prevent deadlock
            if (xSemaphoreTake(spiMutexDisp, pdMS_TO_TICKS(1000)) == pdTRUE) {
                tft.pushImage(xDisp, yDisp, bufferWidth, bufferHeight, buffer2);
                xSemaphoreGive(spiMutexDisp);
                Serial.println("Drawing buffer 2 to display");
                
                // Add frame rate control delay
                vTaskDelay(pdMS_TO_TICKS(66)); // ~15 FPS (more stable)
                
                // Trigger next load after drawing and delay
                xTaskNotifyGive(loadBuffer2TaskHandle); // Notify the load task to start loading the next frame
                active = false; // Reset state to wait for next notification
            } else {
                Serial.println("DrawBuffer2: Failed to acquire display semaphore");
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        else{
            // Wait for the task to be notified to start drawing with timeout
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) > 0) {
                active = true;
                Serial.println("Buffer 2 drawing started");
            }
        }
    }
    vTaskDelete(NULL);
}

void startSDVideo(const char *file_name, int x, int y, int width, int height){

    // Initialize watchdog first
    initializeWatchdog();

    bufferWidth = width;
    bufferHeight = height;

    xDisp = x;
    yDisp = y;

    // Count available frames first
    countAvailableFrames(file_name);
    if (totalFrames < 2) {
        Serial.println("Not enough frames found for video playback");
        return;
    }

    // Initialize frame management
    currentFrameIndex = 1;
    frameIndexMutex = xSemaphoreCreateMutex();
    if (!frameIndexMutex) {
        Serial.println("Failed to create frame index mutex");
        return;
    }

    buffer1 = (uint8_t *)heap_caps_malloc(width*height*sizeof(uint8_t), MALLOC_CAP_DMA);
    buffer2 = (uint8_t *)heap_caps_malloc(width*height*sizeof(uint8_t), MALLOC_CAP_DMA);
    if (!buffer1 || !buffer2) {
        Serial.println("Failed to allocate memory for video buffers");
        if (buffer1) heap_caps_free(buffer1);
        if (buffer2) heap_caps_free(buffer2);
        return;
    }

    spiMutexBuffer = xSemaphoreCreateMutex();
    spiMutexDisp = xSemaphoreCreateMutex();
    if (!spiMutexBuffer || !spiMutexDisp) {
        Serial.println("Failed to create semaphores for video buffers");
        heap_caps_free(buffer1);
        heap_caps_free(buffer2);
        return;
    }

    xTaskCreatePinnedToCore(
        loadBuffer1,           // Task function
        "LoadBuffer1",         // Name of task
        8192,                  // Increased stack size
        (void *) file_name,    // Task input parameter
        2,                     // Priority of the task
        &loadBuffer1TaskHandle,// Task handle
        1                      // Core ID (e.g., 0 or 1)
    );

    xTaskCreatePinnedToCore(
        loadBuffer2,           // Task function
        "LoadBuffer2",         // Name of task
        8192,                  // Increased stack size
        (void *) file_name,    // Task input parameter
        2,                     // Priority of the task
        &loadBuffer2TaskHandle,// Task handle
        1                      // Core ID (e.g., 0 or 1)
    );

    xTaskCreatePinnedToCore(
        drawBuffer1,           // Task function
        "DrawBuffer1",         // Name of task
        8192,                  // Increased stack size
        NULL,                  // Task input parameter
        2,                     // Priority of the task
        &drawBuffer1TaskHandle,// Task handle
        0                      // Core ID (e.g., 0 or 1)
    );

    xTaskCreatePinnedToCore(
        drawBuffer2,           // Task function
        "DrawBuffer2",         // Name of task
        8192,                  // Increased stack size
        NULL,                  // Task input parameter
        2,                     // Priority of the task
        &drawBuffer2TaskHandle,// Task handle
        0                      // Core ID (e.g., 0 or 1)
    );

    // Wait a moment for tasks to initialize
    vTaskDelay(pdMS_TO_TICKS(100));

    // Start the initial loading sequence
    // Load first frame into buffer1, then trigger its draw
    xTaskNotifyGive(loadBuffer1TaskHandle);
    
    // Wait a bit then load second frame into buffer2
    vTaskDelay(pdMS_TO_TICKS(50));
    xTaskNotifyGive(loadBuffer2TaskHandle);
}