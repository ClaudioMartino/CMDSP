#include "wav.hpp"

// Thanks to https://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/

unsigned int little_to_big_endian_4(unsigned char* in) {
  return ( in[0] | (in[1] << 8) | (in[2] << 16) | (in[3] << 24) );
}

unsigned int little_to_big_endian_2(unsigned char* in) {
  return ( in[0] | (in[1] << 8) );
}

void read_wav_header(std::ifstream& fs, WavHeader& header, bool verbose) {
  // Read RIFF chunk descriptor
  fs.read(header.riff, sizeof(header.riff));
  if(verbose)
    std::cout << "[ 0- 3] Chunk ID: " << header.riff << std::endl; 

  fs.read(header.little_chunk_size, 4);
  header.chunk_size = little_to_big_endian_4((unsigned char *)header.little_chunk_size);
  if(verbose)
    std::cout << "[ 4- 7] Chunk Size: " << header.chunk_size << " bytes (" << header.chunk_size / 1024 << " KB)" << std::endl;

  fs.read(header.wave, sizeof(header.wave));
  if(verbose)
    std::cout << "[ 8-11] Format: " << header.wave << std::endl;

  // Read fmt sub-chunk
  fs.read(header.subchunk1_id, sizeof(header.subchunk1_id));
  if(verbose)
    std::cout << "[12-15] Subchunk1 ID: " << header.subchunk1_id << std::endl;

  fs.read(header.little_subchunk1_size, 4);
  header.subchunk1_size = little_to_big_endian_4((unsigned char *)header.little_subchunk1_size);
  if(verbose)
    std::cout << "[16-19] Subchunk1 Size: " << header.subchunk1_size << std::endl;

  fs.read(header.little_audio_format, 2);
  header.audio_format = little_to_big_endian_2((unsigned char *)header.little_audio_format);
  if(verbose) {
    std::cout << "[20-21] Audio Format: " << header.audio_format << " -> ";
    switch(header.audio_format) {
      case(1):
        std::cout << "PCM";
        break;
      case(6):
        std::cout << "A-law";
        break;
      case(7):
        std::cout << "Mu-law";
        break;
    }
    std::cout << std::endl;
  }

  fs.read(header.little_num_channels, 2);
  header.num_channels = little_to_big_endian_2((unsigned char *)header.little_num_channels);
  if(verbose)
    std::cout << "[22-23] Num Channels: "<< header.num_channels << std::endl;

  fs.read(header.little_sample_rate, 4);
  header.sample_rate = little_to_big_endian_4((unsigned char *)header.little_sample_rate);
  //if(verbose)
    std::cout << "[24-27] Sample Rate: " << header.sample_rate << std::endl;

  fs.read(header.little_byte_rate, 4);
  header.byte_rate = little_to_big_endian_4((unsigned char *)header.little_byte_rate);
  if(verbose)
    std::cout << "[28-31] Byte Rate: " << header.byte_rate << " (Bit Rate: " << header.byte_rate * 8 << ")" << std::endl;

  fs.read(header.little_block_align, 2);
  header.block_align = little_to_big_endian_2((unsigned char *)header.little_block_align);
  if(verbose)
    std::cout << "[32-33] Block Align: " << header.block_align << std::endl;

  fs.read(header.little_bits_per_sample, 2);
  header.bits_per_sample = little_to_big_endian_2((unsigned char *)header.little_bits_per_sample);
  if(verbose)
    std::cout << "[34-35] Bits per Sample: " << header.bits_per_sample << std::endl;

  // Read data sub-chunk
  fs.read(header.subchunk2_id, sizeof(header.subchunk2_id));
  if(verbose)
    std::cout << "[36-39] Data Subchunk2 ID: " << header.subchunk2_id << std::endl;

  fs.read(header.little_subchunk2_size, 4);
  header.subchunk2_size = little_to_big_endian_4((unsigned char *)header.little_subchunk2_size);
  if(verbose)
    std::cout << "[40-43] Subchunk2 Size: " << header.subchunk2_size << " bytes (" << header.subchunk2_size / 1024 << " KB)" << std::endl;

  // Check some values
  if(header.audio_format == 1) {
    if(header.subchunk1_size != 16) {
      std::cout << "Error in the header sub-chunk 1 size for PCM." << std::endl;
      exit(1);
    }
  }

  if(header.byte_rate != header.sample_rate * header.num_channels * header.bits_per_sample / 8) {
    std::cout << "Error in the header byte rate." << std::endl;
    exit(1);
  }
  if(header.block_align != header.num_channels * header.bits_per_sample / 8) {
    std::cout << "Error in the header block align." << std::endl;
    exit(1);
  }

  if(header.chunk_size != 4 + (8 + header.subchunk1_size) + (8 + header.subchunk2_size)) {
    std::cout << "Error in the header sub-chunk 2 size." << std::endl;
    exit(1);
  }
}

void compute_wave_sample_sizes(WavHeader& header, long& num_samples, long& size_of_each_sample, bool verbose) {
  long bits_per_channel = 8 * header.subchunk2_size;
  long num_samples_per_channel = bits_per_channel / header.bits_per_sample;
  num_samples = num_samples_per_channel / header.num_channels;
  if(verbose)
    std::cout << "Number of samples: " << num_samples << std::endl;

  size_of_each_sample = header.num_channels * header.bits_per_sample / 8;
  if(verbose)
    std::cout << "Size of each sample: " << size_of_each_sample << " bytes" << std::endl;

  float duration_in_seconds = (float)header.subchunk2_size / header.byte_rate;
  if(verbose)
    std::cout << "Duration: " << duration_in_seconds << " s" << std::endl;
} 

void read_pcm_wav_data(std::ifstream& fs, WavHeader& header, long num_samples, long size_of_each_sample, std::vector<Cpx<double>>& out) {

  if(header.num_channels > 1) {
    std::cout << "More than one channel (" << header.num_channels << "), read not supported for now." << std::endl;
    exit(1);
  }

  char data_buffer[size_of_each_sample];
  long bytes_in_each_channel = size_of_each_sample / header.num_channels;

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

  for (size_t i=0; i<num_samples; i++) {
    fs.read(data_buffer, sizeof(data_buffer));

    int data_in_channel = 0;
    int offset = 0;

    //for(unsigned int xchannels = 0; xchannels < header.num_channels; xchannels++) {

    // Convert data from little endian to big endian based on bytes in each channel sample
    switch(bytes_in_each_channel) {
      case 4:
        data_in_channel = ((data_buffer[offset    ] & 0x00ff)      )
                        | ((data_buffer[offset + 1] & 0x00ff) << 8 )
                        | ((data_buffer[offset + 2] & 0x00ff) << 16)
                        | ((data_buffer[offset + 3]         ) << 24);
        break;
      case 2:
        data_in_channel = ((data_buffer[offset    ] & 0x00ff)     )
                        | ((data_buffer[offset + 1]         ) << 8);
        break;
      case 1:
        data_in_channel = data_buffer[offset] & 0x00ff;
        data_in_channel -= 128; // 8-bit are unsigned, so shifting to signed
        break;
    }

    // Save real input in vector
    out[i] = data_in_channel;

    // Check if sample was in range
    //if (data_in_channel < low_limit || data_in_channel > high_limit)
    //  printf("**value out of range\n");

    //offset += bytes_in_each_channel;    
    //}
  }
}

void write_wav_header(std::ofstream& fs, WavHeader header) {
  fs.write(header.riff, 4);
  fs.write(header.little_chunk_size, 4);
  fs.write(header.wave, 4);
  fs.write(header.subchunk1_id, 4);
  fs.write(header.little_subchunk1_size, 4);
  fs.write(header.little_audio_format, 2);
  fs.write(header.little_num_channels, 2);
  fs.write(header.little_sample_rate, 4);
  fs.write(header.little_byte_rate, 4);
  fs.write(header.little_block_align, 2);
  fs.write(header.little_bits_per_sample, 2);
  fs.write(header.subchunk2_id, 4);
  fs.write(header.little_subchunk2_size, 4);
} 

void write_pcm_wav_data(std::ofstream& fs, WavHeader& header, long num_samples, long size_of_each_sample, std::vector<Cpx<double>> out) {
  char data_buffer[size_of_each_sample];
  long bytes_in_each_channel = size_of_each_sample / header.num_channels;

  for (size_t i=0; i<num_samples; i++) {
    int data = int(real(out[i]));

    // Convert data from big endian to little endian
    switch(bytes_in_each_channel) {
      case 4:
        // TODO
        printf("TODO\n");
        exit(1);
        break;
      case 2:
        data_buffer[0] = (char)(data     );
        data_buffer[1] = (char)(data >> 8);
        break;
      case 1:
        // TODO
        printf("TODO\n");
        exit(1);
        break;
    }

    fs.write(data_buffer, sizeof(data_buffer));
  }
}

void signal_from_wav_file(std::ifstream& fs, WavHeader& header, std::vector<Cpx<double>>& x, bool all) {
  // Read header
  read_wav_header(fs, header, false);

  if (header.audio_format != 1) { // PCM only
    std::cout << "Only PCM is supported." << std::endl;
    exit(1);
  }

  // Compute signal size
  long num_samples, size_of_each_sample;
  compute_wave_sample_sizes(header, num_samples, size_of_each_sample, true);
 
  if(all) {
    x.resize(num_samples);
  
    // Read signal
    read_pcm_wav_data(fs, header, num_samples, size_of_each_sample, x);
  }
  else {
 
    size_t N = x.size();
    if(N > num_samples) {
      std::cout << "N must be smaller than the number of samples in the file ";
      std::cout << "(N = " << N << ", samples = " << num_samples << ")" << std::endl;
      exit(1);
    }
  
    // Read signal
    read_pcm_wav_data(fs, header, N, size_of_each_sample, x);
  }
}
