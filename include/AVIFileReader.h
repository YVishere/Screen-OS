#ifndef __avi_file_reader_h__
#define __avi_file_reader_h__

#include <SD.h>
#include <FS.h>
#include "FrameSource.h"
#include <Arduino.h>

typedef struct
{
    // RIFF Header
    char riff_header[4];      // Contains "RIFF"
    uint32_t file_size;       // Size of the entire file minus 8 bytes
    char file_type[4];        // Contains "AVI "

    // List Header for main data
    char list_header[4];      // Contains "LIST"
    uint32_t list_size;       // Size of the list
    char list_type[4];        // Contains "hdrl"

    // AVI Header
    char avih_header[4];      // Contains "avih"
    uint32_t avih_size;       // Size of AVI header (usually 56)
    uint32_t micro_sec_per_frame;  // Frame rate
    uint32_t max_bytes_per_sec;    // Data rate
    uint32_t padding_granularity;
    uint32_t flags;
    uint32_t total_frames;    // Total number of frames
    uint32_t initial_frames;
    uint32_t streams;         // Number of streams
    uint32_t suggested_buffer_size;
    uint32_t width;           // Frame width
    uint32_t height;          // Frame height
    uint32_t reserved[4];     // Reserved fields
} avi_header_t;

typedef struct
{
    char stream_header[4];    // Contains "strh"
    uint32_t header_size;     // Size of stream header
    char stream_type[4];      // Stream type ("vids" for video)
    char codec[4];            // Codec used
    uint32_t flags;
    uint16_t priority;
    uint16_t language;
    uint32_t initial_frames;
    uint32_t scale;
    uint32_t rate;            // Frame rate = rate/scale
    uint32_t start;
    uint32_t length;          // Number of frames
    uint32_t suggested_buffer_size;
    uint32_t quality;
    uint32_t sample_size;
} stream_header_t;

class AVIFileReader : public FrameSource
{
private:
    int m_frame_rate;
    int m_frame_width;
    int m_frame_height;
    uint32_t m_total_frames;
    uint32_t m_current_frame;
    File m_file;
    uint32_t m_data_start_position;
    VideoFrame_t m_current_video_frame;
    
    void DumpAVIHeader(avi_header_t* avi);
    void PrintData(const char* Data, uint8_t NumBytes);
    bool ValidAviData(avi_header_t* avi);
    bool FindDataChunk();
    bool ReadFrameData(VideoFrame_t *frame);

public:
    AVIFileReader(const char *file_name);
    ~AVIFileReader();
    int frameRate() { return m_frame_rate; }
    int frameWidth() { return m_frame_width; }
    int frameHeight() { return m_frame_height; }
    bool getNextFrame(VideoFrame_t *frame);
    void rewind();
};

#endif
