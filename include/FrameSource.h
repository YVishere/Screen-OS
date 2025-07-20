#ifndef __frame_source_h__
#define __frame_source_h__

#include <Arduino.h>

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t *data;  // RGB565 pixel data
    uint32_t size;  // Size of data in bytes
} VideoFrame_t;

/**
 * Base class for our video frame generators
 **/
class FrameSource
{
public:
    virtual int frameRate() = 0;
    virtual int frameWidth() = 0;
    virtual int frameHeight() = 0;
    // This should fill the frame buffer with the next video frame
    // Frame data should be in RGB565 format (16 bits per pixel)
    virtual bool getNextFrame(VideoFrame_t *frame) = 0;
    virtual void rewind() = 0;
};

#endif
