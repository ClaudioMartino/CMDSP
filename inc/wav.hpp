#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include "complex.hpp"

// WAVE file header format (a RIFF file where Format=WAVE)
// see: http://soundfile.sapp.org/doc/WaveFormat/
struct WavHeader {
  // RIFF chunk descriptor
  char riff[4];                  // 'RIFF' string
  unsigned int chunk_size;       // size of the rest of the file (i.e. file size - 8 bytes)
  char wave[4];                  // 'WAVE' string
  // fmt sub-chunk
  char subchunk1_id[4];          // 'fmt ' string (with trailing null char)
  unsigned int subchunk1_size;   // size of the rest of the sub-chunk (i.e. sub-chunk size - 8 bytes)
  unsigned int audio_format;     // format type: 1=PCM, 3=IEEE float, 6=8bit A law, 7=8bit mu law
  unsigned int num_channels;     // number of channels: 1=mono, 2=stereo
  unsigned int sample_rate;      // sampling rate (blocks per second)
  unsigned int byte_rate;        // sample_rate * num_channels * bits_per_sample / 8
  unsigned int block_align;      // num_channels * bits_per_sample / 8
  unsigned int bits_per_sample;  // bits per sample (8, 16...)
  // data sub-chunk
  char subchunk2_id[4];          // 'DATA' string or 'FLLR' string
  unsigned int subchunk2_size;   // size of data in bytes: num_samples * num_channels * bits_per_sample / 8

  // Little endian values
  char little_chunk_size[4];
  char little_subchunk1_size[4];
  char little_audio_format[2];
  char little_num_channels[2];
  char little_sample_rate[4];
  char little_byte_rate[4];
  char little_block_align[2];
  char little_bits_per_sample[2];
  char little_subchunk2_size[4];
};

unsigned int little_to_big_endian_4(unsigned char* in);
unsigned int little_to_big_endian_2(unsigned char* in);
void read_wav_header(std::ifstream& fs, WavHeader& header, bool verbose);
void compute_wave_sample_sizes(WavHeader& header, long& num_samples, long& size_of_each_sample, bool verbose);
void read_pcm_wav_data(std::ifstream& fs, WavHeader& header, long num_samples, long size_of_each_sample, std::vector<Cpx<double>>& out);
void write_wav_header(std::ofstream& fs, WavHeader header);
void write_pcm_wav_data(std::ofstream& fs, WavHeader& header, long num_samples, long size_of_each_sample, std::vector<Cpx<double>> out);
void signal_from_wav_file(std::ifstream& fs, WavHeader& header, std::vector<Cpx<double>>& x, bool all);
