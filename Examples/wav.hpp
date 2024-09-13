#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Thanks to https://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/

// WAVE file header format (a RIFF file where Format=WAVE)
// see: http://soundfile.sapp.org/doc/WaveFormat/
struct WavHeader {
  // RIFF chunk descriptor
  unsigned char riff[4];          // ChunkID (RIFF string)
  unsigned int  chunk_size;       // ChunkSize (overall size of file in bytes)
  unsigned char wave[4];          // Format (WAVE string)
  // fmt sub-chunk
  unsigned char subchunk1_id[4];  // fmt string with trailing null char
  unsigned int  subchunk1_size;   // length of the format data
  unsigned int  audio_format;     // format type: 1- PCM, 3- IEEE float, 6- 8bit A law, 7- 8bit mu law
  unsigned int  num_channels;     // number of channels
  unsigned int  sample_rate;      // sampling rate (blocks per second)
  unsigned int  byte_rate;        // sample_rate * num_channels * bits_per_sample / 8
  unsigned int  block_align;      // num_channels * bits_per_sample / 8
  unsigned int  bits_per_sample;  // bits per sample (8, 16...)
  // data sub-chunk
  unsigned char subchunk2_id[4];  // DATA string or FLLR string
  unsigned int  subchunk2_size;   // num_samples * num_channels * bits_per_sample / 8
};

unsigned int little_to_big_endian_4(unsigned char* in) {
  return ( in[0] | (in[1] << 8) | (in[2] << 16) | (in[3] << 24) );
}

unsigned int little_to_big_endian_2(unsigned char* in) {
  return ( in[0] | (in[1] << 8) );
}

void read_wav_header(FILE* ptr, WavHeader& header, bool verbose) {
  // Buffers used to do big endian conversion 
  unsigned char buffer4[4];
  unsigned char buffer2[2];

  // Read RIFF chunk descriptor
  int read = fread(header.riff, sizeof(header.riff), 1, ptr);
  if(verbose)
    printf("[ 0- 3] Chunk ID: %s \n", header.riff); 

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header.chunk_size = little_to_big_endian_4(buffer4);
  if(verbose)
    printf("[ 4- 7] Chunk Size: %u bytes (%u KB)\n", header.chunk_size, header.chunk_size/1024);

  read = fread(header.wave, sizeof(header.wave), 1, ptr);
  if(verbose)
    printf("[ 8-11] Format: %s\n", header.wave);

  // Read fmt sub-chunk
  read = fread(header.subchunk1_id, sizeof(header.subchunk1_id), 1, ptr);
  if(verbose)
    printf("[12-15] Subchunk1 ID: %s\n", header.subchunk1_id);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header.subchunk1_size = little_to_big_endian_4(buffer4);
  if(verbose)
    printf("[16-19] Subchunk1 Size: %u\n", header.subchunk1_size);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header.audio_format = little_to_big_endian_2(buffer2);
  if(verbose) {
    printf("[20-21] Audio Format: %u -> ", header.audio_format);
    if (header.audio_format == 1)
      printf("PCM\n"); 
    else if (header.audio_format == 6)
      printf("A-law\n");
    else if (header.audio_format == 7)
      printf("Mu-law\n");
  }

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header.num_channels = little_to_big_endian_2(buffer2);
  if(verbose)
    printf("[22-23] Num Channels: %u \n", header.num_channels);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header.sample_rate = little_to_big_endian_4(buffer4);
  //if(verbose)
  printf("[24-27] Sample Rate: %u\n", header.sample_rate);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header.byte_rate = little_to_big_endian_4(buffer4);
  if(verbose)
    printf("[28-31] Byte Rate: %u, Bit Rate: %u\n", header.byte_rate, header.byte_rate*8);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header.block_align = little_to_big_endian_2(buffer2);
  if(verbose)
    printf("[32-33] Block Align: %u \n", header.block_align);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header.bits_per_sample = little_to_big_endian_2(buffer2);
  if(verbose)
    printf("[34-35] Bits Per Sample: %u \n", header.bits_per_sample);

  // Read data sub-chunk
  read = fread(header.subchunk2_id, sizeof(header.subchunk2_id), 1, ptr);
  if(verbose)
    printf("[36-39] Data Subchunk2 ID: %s \n", header.subchunk2_id);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header.subchunk2_size = little_to_big_endian_4(buffer4);
  if(verbose)
    printf("[40-43] Subchunk2 Size: %u \n", header.subchunk2_size);
}

void compute_wave_sample_sizes(FILE* ptr, WavHeader& header, long& num_samples, long& size_of_each_sample) {
  num_samples = (8 * header.subchunk2_size) / (header.num_channels * header.bits_per_sample);
  //printf("Number of samples: %lu \n", num_samples);
  size_of_each_sample = (header.num_channels * header.bits_per_sample) / 8;
  //printf("Size of each sample: %ld bytes\n", size_of_each_sample);
  //float duration_in_seconds = (float)header.chunk_size / header.byte_rate;
  //printf("Approx. duration in seconds: %f\n", duration_in_seconds);
} 

void read_pcm_wav_data(FILE* ptr, WavHeader& header, long num_samples, long size_of_each_sample, std::vector<Cpx<double>>& out) {

  if(header.num_channels > 1) {
    printf("More than one channel, not supported.\n");
    exit(1);
  }

  char data_buffer[size_of_each_sample];
  long bytes_in_each_channel = (size_of_each_sample / header.num_channels);

  // Make sure that the bytes-per-sample is completely divisible by num. of channels
  //if((bytes_in_each_channel * header.num_channels) != size_of_each_sample) {
  //  printf("Error: %ld x %ud <> %ld\n", bytes_in_each_channel, header.num_channels, size_of_each_sample);
  //  exit(1);
  //}

  // The valid amplitude range for values based on the bits per sample
  //long low_limit = 0l;
  //switch(header.bits_per_sample) {
  //  case 8:
  //    low_limit = -128;
  //    break;
  //  case 16:
  //    low_limit = -32768;
  //    break;
  //  case 32:
  //    low_limit = -2147483648;
  //    break;
  //}          
  //long high_limit = -low_limit - 1;

  int read = 0;

  for (long i=0; i<num_samples; i++) {
    read = fread(data_buffer, sizeof(data_buffer), 1, ptr);

    if (read == 1) {
      int data_in_channel = 0;
      int offset = 0;

      //for(unsigned int xchannels = 0; xchannels < header.num_channels; xchannels++) {

      // Convert data from little endian to big endian based on bytes in each channel sample
      switch(bytes_in_each_channel) {
        case 4:
          data_in_channel = (data_buffer[offset] & 0x00ff) | ((data_buffer[offset + 1] & 0x00ff) << 8) | ((data_buffer[offset + 2] & 0x00ff) << 16) | (data_buffer[offset + 3] << 24);
          break;
        case 2:
          data_in_channel = (data_buffer[offset] & 0x00ff) | (data_buffer[offset + 1] << 8);
          break;
        case 1:
          data_in_channel = data_buffer[offset] & 0x00ff;
          data_in_channel -= 128; // 8-bit are unsigned, so shifting to signed
          break;
      }

      // Save real input in vector
      Cpx<double> c {(double)data_in_channel, 0};
      out[i] = data_in_channel;

      // Check if sample was in range
      //if (data_in_channel < low_limit || data_in_channel > high_limit)
      //  printf("**value out of range\n");

      //offset += bytes_in_each_channel;    
      //}
    }
    else {
      printf("Error reading file. %d bytes\n", read);
      exit(1);
    }
  }
}
