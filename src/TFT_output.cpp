#include <Arduino.h>
#include <TFT_eSPI.h>
#include <math.h>

#include "FrameSource.h"
#include "TFT_output.h"

// Event types for TFT display queue
#define TFT_EVENT_DISPLAY_FRAME 1

typedef struct {
    int type;
} tft_event_t;

void tftDisplayTask(void *param)
{
    TFT_Output *output = (TFT_Output *)param;
    VideoFrame_t current_frame;
    current_frame.data = nullptr;
    current_frame.size = 0;
    
    unsigned long frame_interval = 1000 / output->m_frame_generator->frameRate(); // ms per frame
    unsigned long last_frame_time = millis();
    
    Serial.printf("TFT Display Task started. Frame rate: %d FPS, interval: %d ms\n", 
                  output->m_frame_generator->frameRate(), frame_interval);
    
    while (true)
    {
        unsigned long current_time = millis();
        
        // Check if it's time for the next frame
        if (current_time - last_frame_time >= frame_interval)
        {
            // Get the next frame from the source
            if (output->m_frame_generator->getNextFrame(&current_frame))
            {
                // Display the frame on TFT
                if (current_frame.data != nullptr && current_frame.size > 0)
                {
                    // For now, assume the frame data is in RGB565 format
                    // Push the frame data directly to the TFT display
                    output->m_tft->setAddrWindow(output->m_display_x, output->m_display_y, 
                                               output->m_display_width, output->m_display_height);
                    
                    // Calculate expected frame size in RGB565 format (2 bytes per pixel)
                    uint32_t expected_size = output->m_display_width * output->m_display_height * 2;
                    
                    if (current_frame.size >= expected_size)
                    {
                        // Push RGB565 data directly to display
                        output->m_tft->pushColors((uint16_t*)current_frame.data, 
                                                output->m_display_width * output->m_display_height);
                    }
                    else
                    {
                        // Handle smaller frame or different format
                        Serial.printf("Frame size mismatch: expected %d, got %d\n", expected_size, current_frame.size);
                        
                        // Fill with a test pattern or scale the available data
                        uint16_t test_color = random(0xFFFF);
                        output->m_tft->fillRect(output->m_display_x, output->m_display_y,
                                              output->m_display_width, output->m_display_height, test_color);
                    }
                }
                last_frame_time = current_time;
            }
            else
            {
                // Failed to get frame, maybe rewind and try again
                Serial.println("Failed to get next frame, rewinding...");
                output->m_frame_generator->rewind();
                delay(100); // Small delay before retry
            }
        }
        else
        {
            // Wait for the next frame time
            delay(1);
        }
        
        // Yield to other tasks occasionally
        if (current_time % 100 == 0) {
            vTaskDelay(1);
        }
    }
    
    // Cleanup
    if (current_frame.data != nullptr) {
        free(current_frame.data);
    }
}

void TFT_Output::start(TFT_eSPI *tft, FrameSource *frame_generator, int x, int y, int width, int height)
{
    m_tft = tft;
    m_frame_generator = frame_generator;
    m_display_x = x;
    m_display_y = y;
    m_display_width = width;
    m_display_height = height;
    m_last_frame_time = millis();
    
    // Calculate frame interval in milliseconds
    m_frame_interval_ms = 1000 / m_frame_generator->frameRate();
    
    Serial.printf("Starting TFT Output: %dx%d at (%d,%d), %d FPS\n", 
                  width, height, x, y, m_frame_generator->frameRate());
    
    // Create TFT display queue (not strictly needed for this implementation but following I2S pattern)
    m_tftQueue = xQueueCreate(4, sizeof(tft_event_t));
    
    // Clear the display area
    m_tft->fillRect(m_display_x, m_display_y, m_display_width, m_display_height, TFT_BLACK);
    
    // Start a task to display frames on the TFT
    xTaskCreate(tftDisplayTask, "TFT Display Task", 8192, this, 2, &m_tftDisplayTaskHandle);
    
    Serial.println("TFT Output started successfully");
}

void TFT_Output::stop()
{
    if (m_tftDisplayTaskHandle != nullptr)
    {
        vTaskDelete(m_tftDisplayTaskHandle);
        m_tftDisplayTaskHandle = nullptr;
    }
    
    if (m_tftQueue != nullptr)
    {
        vQueueDelete(m_tftQueue);
        m_tftQueue = nullptr;
    }
    
    Serial.println("TFT Output stopped");
}
