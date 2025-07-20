#include <SD.h>
#include <FS.h>
#include "AVIFileReader.h"

void AVIFileReader::PrintData(const char* Data, uint8_t NumBytes)
{
    for(uint8_t i = 0; i < NumBytes; i++)
        Serial.print(Data[i]); 
    Serial.println();  
}

bool AVIFileReader::ValidAviData(avi_header_t* avi)
{
    if(memcmp(avi->riff_header, "RIFF", 4) != 0) 
    {    
        Serial.print("Invalid data - Not RIFF format");
        return false;        
    }
    if(memcmp(avi->file_type, "AVI ", 4) != 0)
    {
        Serial.print("Invalid data - Not AVI file");
        return false;           
    }
    if(memcmp(avi->list_header, "LIST", 4) != 0) 
    {
        Serial.print("Invalid data - No LIST section found");
        return false;       
    }
    if(memcmp(avi->list_type, "hdrl", 4) != 0) 
    {
        Serial.print("Invalid data - No hdrl section found");
        return false;      
    }
    if(memcmp(avi->avih_header, "avih", 4) != 0) 
    {
        Serial.print("Invalid data - No avih header found");
        return false;                          
    }
    if(avi->width == 0 || avi->height == 0)
    {
        Serial.print("Invalid data - Invalid frame dimensions");
        return false;   
    }
    if(avi->total_frames == 0) 
    {
        Serial.print("Invalid data - No frames found");
        return false;                       
    }
    return true;
}

void AVIFileReader::DumpAVIHeader(avi_header_t* avi)
{
    if(memcmp(avi->riff_header, "RIFF", 4) != 0)
    {
        Serial.print("Not a RIFF format file - ");    
        PrintData(avi->riff_header, 4);
        return;
    } 
    if(memcmp(avi->file_type, "AVI ", 4) != 0)
    {
        Serial.print("Not an AVI file - ");  
        PrintData(avi->file_type, 4);  
        return;
    }  
    if(memcmp(avi->avih_header, "avih", 4) != 0)
    {
        Serial.print("avih ID not present - ");
        PrintData(avi->avih_header, 4);      
        return;
    } 
    
    // All looks good, dump the data
    Serial.print("File size: "); Serial.println(avi->file_size);
    Serial.print("Frame rate (microsec per frame): "); Serial.println(avi->micro_sec_per_frame);
    Serial.print("Max bytes per sec: "); Serial.println(avi->max_bytes_per_sec);
    Serial.print("Total frames: "); Serial.println(avi->total_frames);
    Serial.print("Streams: "); Serial.println(avi->streams);
    Serial.print("Width: "); Serial.println(avi->width);
    Serial.print("Height: "); Serial.println(avi->height);
}

bool AVIFileReader::FindDataChunk()
{
    char chunk_header[4];
    uint32_t chunk_size;
    
    // Reset to beginning and skip RIFF header
    m_file.seek(12); // Skip RIFF header (12 bytes)
    
    while(m_file.available() > 8) {
        if(m_file.read((byte*)chunk_header, 4) != 4) break;
        if(m_file.read((byte*)&chunk_size, 4) != 4) break;
        
        if(memcmp(chunk_header, "LIST", 4) == 0) {
            // Check if this is the movi list
            char list_type[4];
            if(m_file.read((byte*)list_type, 4) == 4) {
                if(memcmp(list_type, "movi", 4) == 0) {
                    m_data_start_position = m_file.position();
                    Serial.printf("Found movi chunk at position: %d\n", m_data_start_position);
                    return true;
                }
            }
            // Skip the rest of this LIST chunk
            m_file.seek(m_file.position() + chunk_size - 4);
        } else {
            // Skip this chunk
            m_file.seek(m_file.position() + chunk_size);
        }
    }
    
    Serial.println("Could not find movi data chunk");
    return false;
}

bool AVIFileReader::ReadFrameData(VideoFrame_t *frame)
{
    char chunk_id[4];
    uint32_t chunk_size;
    
    // Read chunk header
    if(m_file.read((byte*)chunk_id, 4) != 4) return false;
    if(m_file.read((byte*)&chunk_size, 4) != 4) return false;
    
    // Check if this is a video frame chunk (usually "00db" or "00dc")
    if(chunk_id[2] == 'd' && (chunk_id[3] == 'b' || chunk_id[3] == 'c')) {
        // Allocate buffer for frame data if needed
        if(frame->data == nullptr || frame->size < chunk_size) {
            if(frame->data != nullptr) {
                free(frame->data);
            }
            frame->data = (uint8_t*)malloc(chunk_size);
            frame->size = chunk_size;
        }
        
        // Read frame data
        if(m_file.read(frame->data, chunk_size) == chunk_size) {
            frame->width = m_frame_width;
            frame->height = m_frame_height;
            return true;
        }
    } else {
        // Skip non-video chunks
        m_file.seek(m_file.position() + chunk_size);
        return ReadFrameData(frame); // Recursive call to find next video frame
    }
    
    return false;
}

AVIFileReader::AVIFileReader(const char *file_name)
{
    m_frame_rate = 0;
    m_frame_width = 0;
    m_frame_height = 0;
    m_total_frames = 0;
    m_current_frame = 0;
    m_data_start_position = 0;
    m_current_video_frame.data = nullptr;
    m_current_video_frame.size = 0;
    
    if (!SD.exists(file_name))
    {
        Serial.println("****** Failed to open AVI file! File does not exist");
        return;
    }
    
    m_file = SD.open(file_name, FILE_READ);
    if (!m_file){
        Serial.println("Failed to open AVI file");
        return;
    }
    
    Serial.printf("Opened AVI file %s, size %d bytes\n", file_name, m_file.size());
    
    // Read the AVI header - simplified version
    avi_header_t avi_header;
    if(m_file.read((byte *)&avi_header, sizeof(avi_header_t)) != sizeof(avi_header_t)) {
        Serial.println("Failed to read AVI header");
        return;
    }
    
    // Validate basic AVI structure
    if(!ValidAviData(&avi_header)) {
        Serial.println("Invalid AVI file format");
        return;
    }
    
    DumpAVIHeader(&avi_header);
    
    // Extract frame information
    m_frame_width = avi_header.width;
    m_frame_height = avi_header.height;
    m_total_frames = avi_header.total_frames;
    
    // Calculate frame rate (frames per second)
    if(avi_header.micro_sec_per_frame > 0) {
        m_frame_rate = 1000000 / avi_header.micro_sec_per_frame;
    } else {
        m_frame_rate = 25; // Default to 25 FPS
    }
    
    // Find the data chunk containing video frames
    if(!FindDataChunk()) {
        Serial.println("Could not locate video data in AVI file");
        return;
    }
    
    Serial.printf("AVI file loaded successfully: %dx%d, %d frames, %d FPS\n", 
                  m_frame_width, m_frame_height, m_total_frames, m_frame_rate);
}

AVIFileReader::~AVIFileReader()
{
    if(m_current_video_frame.data != nullptr) {
        free(m_current_video_frame.data);
    }
    m_file.close();
}

bool AVIFileReader::getNextFrame(VideoFrame_t *frame)
{
    if(m_current_frame >= m_total_frames) {
        rewind(); // Loop back to beginning
    }
    
    if(ReadFrameData(frame)) {
        m_current_frame++;
        return true;
    }
    
    return false;
}

void AVIFileReader::rewind()
{
    m_current_frame = 0;
    m_file.seek(m_data_start_position);
}
