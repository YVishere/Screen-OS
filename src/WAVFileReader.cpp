#include <SD.h>
#include <FS.h>
#include "WAVFileReader.h"


void WAVFileReader::PrintData(const char* Data,uint8_t NumBytes)
{
    for(uint8_t i=0;i<NumBytes;i++)
      Serial.print(Data[i]); 
      Serial.println();  
}

bool WAVFileReader::ValidWavData(wav_header_t* Wav)
{
  
  if(memcmp(Wav->riff_header,"RIFF",4)!=0) 
  {    
    Serial.print("Invalid data - Not RIFF format");
    return false;        
  }
  if(memcmp(Wav->wave_header,"WAVE",4)!=0)
  {
    Serial.print("Invalid data - Not Wave file");
    return false;           
  }
  if(memcmp(Wav->fmt_header,"fmt",3)!=0) 
  {
    Serial.print("Invalid data - No format section found");
    return false;       
  }
  if(memcmp(Wav->data_header,"data",4)!=0) 
  {
    Serial.print("Invalid data - data section not found");
    return false;      
  }
  if(Wav->audio_format!=1) 
  {
    Serial.print("Invalid data - format Id must be 1");
    return false;                          
  }
  if(Wav->fmt_chunk_size!=16) 
  {
    Serial.print("Invalid data - format section size must be 16.");
    return false;                          
  }
  if((Wav->num_channels!=1)&(Wav->num_channels!=2))
  {
    Serial.print("Invalid data - only mono or stereo permitted.");
    return false;   
  }
  if(Wav->sample_rate>48000) 
  {
    Serial.print("Invalid data - Sample rate cannot be greater than 48000");
    return false;                       
  }
  if((Wav->bit_depth!=8)& (Wav->bit_depth!=16)) 
  {
    Serial.print("Invalid data - Only 8 or 16 bits per sample permitted.");
    return false;                        
  }
  return true;
}

void WAVFileReader::DumpWAVHeader(wav_header_t* Wav)
{
  if(memcmp(Wav->riff_header,"RIFF",4)!=0)
  {
    Serial.print("Not a RIFF format file - ");    
    PrintData(Wav->riff_header,4);
    return;
  } 
  if(memcmp(Wav->wave_header,"WAVE",4)!=0)
  {
    Serial.print("Not a WAVE file - ");  
    PrintData(Wav->wave_header,4);  
    return;
  }  
  if(memcmp(Wav->fmt_header,"fmt",3)!=0)
  {
    Serial.print("fmt ID not present - ");
    PrintData(Wav->fmt_header,3);      
    return;
  } 
  if(memcmp(Wav->data_header,"data",4)!=0)
  {
    Serial.print("data ID not present - "); 
    PrintData(Wav->data_header,4);
    return;
  }  
  // All looks good, dump the data
  Serial.print("Total size :");Serial.println(Wav->wav_size);
  Serial.print("Format section size :");Serial.println(Wav->fmt_chunk_size);
  Serial.print("Wave format :");Serial.println(Wav->audio_format);
  Serial.print("Channels :");Serial.println(Wav->num_channels);
  Serial.print("Sample Rate :");Serial.println(Wav->sample_rate);
  Serial.print("Byte Rate :");Serial.println(Wav->byte_rate);
  Serial.print("Block Align :");Serial.println(Wav->sample_alignment);
  Serial.print("Bits Per Sample :");Serial.println(Wav->bit_depth);
  Serial.print("Data Size :");Serial.println(Wav->data_bytes);
}

WAVFileReader::WAVFileReader(const char *file_name)
{
    if (!SD.exists(file_name))
    {
        Serial.println("****** Failed to open file! Have you uploaed the file system?");
        return;
    }
    m_file = SD.open(file_name, FILE_READ);

    if (!m_file){
        Serial.println("Failed to open wav file");
        return;
    }
    Serial.printf("Opened file %s, size %d bytes\n", file_name, m_file.size());
    
    // read the WAV header
    wav_header_t wav_header;
    Serial.printf("wav_header size %d\n", sizeof(wav_header_t));
    m_file.read((byte *)&wav_header, sizeof(wav_header_t));

    DumpWAVHeader(&wav_header);

    // sanity check the bit depth
    if (!ValidWavData(&wav_header)){
        Serial.println("Invalid .wav file");
    }

    m_num_channels = wav_header.num_channels;
    m_sample_rate = wav_header.sample_rate;
}

WAVFileReader::~WAVFileReader()
{
    m_file.close();
}

void WAVFileReader::getFrames(Frame_t *frames, int number_frames)
{
    // fill the buffer with data from the file wrapping around if necessary
    for (int i = 0; i < number_frames; i++)
    {
        // if we've reached the end of the file then seek back to the beginning (after the header)
        if (m_file.available() == 0)
        {
            m_file.seek(44);
        }
        // read in the next sample to the left channel
        m_file.read((uint8_t *)(&frames[i].left), sizeof(int16_t));
        // if we only have one channel duplicate the sample for the right channel
        if (m_num_channels == 1)
        {
            frames[i].right = frames[i].left;
        }
        else
        {
            // otherwise read in the right channel sample
            m_file.read((uint8_t *)(&frames[i].right), sizeof(int16_t));
        }
    }
}