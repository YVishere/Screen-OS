#ifndef __wav_file_reader_h__
#define __wav_file_reader_h__

#include <SD.h>
#include <FS.h>
#include "SampleSource.h"
#include <Arduino.h>

typedef struct
{
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    uint32_t wav_size;        // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    uint32_t fmt_chunk_size; // Should be 16 for PCM
    uint16_t audio_format; // Should be 1 for PCM. 3 for IEEE Float
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;          // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    uint16_t sample_alignment; // num_channels * Bytes Per Sample
    uint16_t bit_depth;        // Number of bits per sample

    // Data
    char data_header[4]; // Contains "data"
    uint32_t data_bytes;      // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} wav_header_t;

class WAVFileReader : public SampleSource
{
private:
    int m_num_channels;
    int m_sample_rate;
    File m_file;
    void DumpWAVHeader(wav_header_t* Wav);
    void PrintData(const char* Data,uint8_t NumBytes);
    bool ValidWavData(wav_header_t* Wav);

public:
    WAVFileReader(const char *file_name);
    ~WAVFileReader();
    int sampleRate() { return m_sample_rate; }
    void getFrames(Frame_t *frames, int number_frames);
};

#endif