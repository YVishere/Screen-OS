# AVIFileReader and TFT_Output Classes

This document describes the new video playback classes that work similarly to the existing WAVFileReader and I2SOutput classes.

## Overview

The video playback system consists of:
- `FrameSource.h` - Base interface for video frame sources (similar to SampleSource.h)
- `AVIFileReader.h/.cpp` - Reads AVI video files from SD card (similar to WAVFileReader)
- `TFT_output.h/.cpp` - Outputs video frames to TFT display using FreeRTOS (similar to I2SOutput)

## Usage

### Basic Setup (similar to audio setup)

```cpp
#include "AVIFileReader.h"
#include "TFT_output.h"

// Global variables
TFT_Output *videoOutput;
FrameSource *videoSource;
extern TFT_eSPI tft; // Declared in display.cpp

void setupVideoPlayback() {
    // Create video source (AVI file reader)
    videoSource = new AVIFileReader("/video.avi");
    
    // Create video output (TFT display)
    videoOutput = new TFT_Output();
    
    // Start video playback on TFT display
    // Parameters: TFT instance, video source, x, y, width, height
    videoOutput->start(&tft, videoSource, 0, 0, 160, 128);
}
```

### Comparison with Audio System

| Audio System | Video System |
|--------------|--------------|
| `SampleSource` | `FrameSource` |
| `WAVFileReader` | `AVIFileReader` |
| `I2SOutput` | `TFT_Output` |
| `Frame_t` (left/right samples) | `VideoFrame_t` (width/height/data) |

### Classes

#### FrameSource (Base Interface)
```cpp
class FrameSource {
public:
    virtual int frameRate() = 0;
    virtual int frameWidth() = 0;
    virtual int frameHeight() = 0;
    virtual bool getNextFrame(VideoFrame_t *frame) = 0;
    virtual void rewind() = 0;
};
```

#### AVIFileReader
- Reads AVI files from SD card
- Supports basic AVI format with video streams
- Automatically loops when reaching end of file
- RGB565 format output (16 bits per pixel)

#### TFT_Output
- Uses FreeRTOS task for frame display (similar to I2S audio task)
- Maintains proper frame timing based on video frame rate
- Non-DMA implementation as requested
- Configurable display position and size

### File Format Support

Currently supports:
- Basic AVI files with uncompressed video streams
- RGB565 pixel format
- Standard frame rates (calculated from AVI header)

### Memory Management

The system handles memory allocation for video frames automatically:
- Frame buffers are allocated as needed
- Memory is freed when AVIFileReader is destroyed
- Uses malloc/free (no DMA buffers)

### Integration Example

In `main.cpp`, uncomment the video playback setup:
```cpp
void setup() {
    // ... existing setup code ...
    
    // For video playback:
    setupVideoPlayback();
}
```

### Notes

1. Ensure your AVI file is in RGB565 format or the display may show incorrect colors
2. The TFT display task runs at priority 2 (higher than default)
3. Frame timing is maintained automatically based on the AVI file's frame rate
4. The system will automatically rewind and loop the video when it reaches the end

### File Locations

- Headers: `include/FrameSource.h`, `include/AVIFileReader.h`, `include/TFT_output.h`
- Implementation: `src/AVIFileReader.cpp`, `src/TFT_output.cpp`
- Example usage: `src/main.cpp` (commented examples)
