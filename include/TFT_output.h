#ifndef __tft_output_h__
#define __tft_output_h__

#include <Arduino.h>
#include <TFT_eSPI.h>

class FrameSource;

/**
 * TFT Display Output for video frames using FreeRTOS
 **/
class TFT_Output
{
private:
    // TFT display task
    TaskHandle_t m_tftDisplayTaskHandle;
    // TFT display queue
    QueueHandle_t m_tftQueue;
    // TFT display instance
    TFT_eSPI *m_tft;
    // Source of video frames for us to display
    FrameSource *m_frame_generator;
    // Display position and size
    int m_display_x;
    int m_display_y;
    int m_display_width;
    int m_display_height;
    // Frame timing
    unsigned long m_last_frame_time;
    unsigned long m_frame_interval_ms;

public:
    void start(TFT_eSPI *tft, FrameSource *frame_generator, int x = 0, int y = 0, int width = 160, int height = 128);
    void stop();
    
    friend void tftDisplayTask(void *param);
};

#endif
