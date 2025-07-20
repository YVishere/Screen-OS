#include "FrameUtils.h"
#include <Arduino.h>

uint16_t FrameUtils::rgb888ToRgb565(uint8_t r, uint8_t g, uint8_t b)
{
    // Convert 8-bit RGB to 5-6-5 format
    // Red: 8 bits -> 5 bits (shift right by 3)
    // Green: 8 bits -> 6 bits (shift right by 2)
    // Blue: 8 bits -> 5 bits (shift right by 3)
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

bool FrameUtils::convertRgb888ToRgb565(uint8_t* rgb888_data, uint16_t* rgb565_data, uint32_t pixel_count)
{
    if (rgb888_data == nullptr || rgb565_data == nullptr) {
        return false;
    }
    
    for (uint32_t i = 0; i < pixel_count; i++) {
        uint8_t r = rgb888_data[i * 3];
        uint8_t g = rgb888_data[i * 3 + 1];
        uint8_t b = rgb888_data[i * 3 + 2];
        
        rgb565_data[i] = rgb888ToRgb565(r, g, b);
    }
    
    return true;
}

bool FrameUtils::scaleFrame(VideoFrame_t* source, VideoFrame_t* dest, int target_width, int target_height)
{
    if (source == nullptr || dest == nullptr || source->data == nullptr) {
        return false;
    }
    
    // Allocate destination buffer if needed
    uint32_t dest_size = target_width * target_height * 2; // 2 bytes per pixel for RGB565
    if (dest->data == nullptr || dest->size < dest_size) {
        if (dest->data != nullptr) {
            free(dest->data);
        }
        dest->data = (uint8_t*)malloc(dest_size);
        if (dest->data == nullptr) {
            return false;
        }
        dest->size = dest_size;
    }
    
    dest->width = target_width;
    dest->height = target_height;
    
    uint16_t* src_pixels = (uint16_t*)source->data;
    uint16_t* dest_pixels = (uint16_t*)dest->data;
    
    // Simple nearest neighbor scaling
    float x_scale = (float)source->width / target_width;
    float y_scale = (float)source->height / target_height;
    
    for (int y = 0; y < target_height; y++) {
        for (int x = 0; x < target_width; x++) {
            int src_x = (int)(x * x_scale);
            int src_y = (int)(y * y_scale);
            
            // Bounds checking
            if (src_x >= source->width) src_x = source->width - 1;
            if (src_y >= source->height) src_y = source->height - 1;
            
            int src_index = src_y * source->width + src_x;
            int dest_index = y * target_width + x;
            
            dest_pixels[dest_index] = src_pixels[src_index];
        }
    }
    
    return true;
}

void FrameUtils::createTestPattern(VideoFrame_t* frame, int width, int height, uint16_t color1, uint16_t color2)
{
    if (frame == nullptr) return;
    
    uint32_t frame_size = width * height * 2; // 2 bytes per pixel for RGB565
    if (frame->data == nullptr || frame->size < frame_size) {
        if (frame->data != nullptr) {
            free(frame->data);
        }
        frame->data = (uint8_t*)malloc(frame_size);
        if (frame->data == nullptr) {
            return;
        }
        frame->size = frame_size;
    }
    
    frame->width = width;
    frame->height = height;
    
    uint16_t* pixels = (uint16_t*)frame->data;
    
    // Create a checkerboard pattern
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            // 8x8 checkerboard pattern
            if (((x / 8) + (y / 8)) % 2 == 0) {
                pixels[index] = color1;
            } else {
                pixels[index] = color2;
            }
        }
    }
}
