#ifndef __frame_utils_h__
#define __frame_utils_h__

#include <Arduino.h>
#include "FrameSource.h"

/**
 * Utility functions for video frame processing
 **/
class FrameUtils
{
public:
    // Convert RGB888 (24-bit) to RGB565 (16-bit)
    static uint16_t rgb888ToRgb565(uint8_t r, uint8_t g, uint8_t b);
    
    // Convert entire frame from RGB888 to RGB565
    static bool convertRgb888ToRgb565(uint8_t* rgb888_data, uint16_t* rgb565_data, uint32_t pixel_count);
    
    // Scale frame to fit display dimensions (simple nearest neighbor)
    static bool scaleFrame(VideoFrame_t* source, VideoFrame_t* dest, int target_width, int target_height);
    
    // Create a test pattern frame (useful for debugging)
    static void createTestPattern(VideoFrame_t* frame, int width, int height, uint16_t color1, uint16_t color2);
};

#endif
