#include "fft.hpp"
#include "wav.hpp"

int main(int argc, char** argv) {

  // Default values
  size_t N = 1024;
  size_t r = 2;
  bool save = false;
  bool bench = false;
  size_t total_rep = 1;
  bool read_from_file = false;
  char * filename = nullptr;

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:r:f:sbh")) {
      case 'n':
        N = atoi(optarg);
        continue;
      case 'r':
        r = atoi(optarg);
        continue;
      case 'f':
        read_from_file = true;
        filename = optarg;
        continue;
      case 's':
        save = true;
        continue;
      case 'b':
        bench = true;
        total_rep = 99;
        continue;
      case 'h':
      default :
        printf("Usage: fft [-n FFT-size] [-r radix-size] [-f file.wav] [-s] [-b]\n");
        return 0;
        break;
      case -1:
        break;
    }
    break;
  }

  // Input signal
  std::vector<Cpx<double>> x(N);

  if(read_from_file) {
    // Open .wav file
    printf("Opening .wav file.\n");

    FILE *ptr = fopen(filename, "rb");
    if (ptr == NULL) {
     printf("Error opening %s.\n", filename);
     exit(1);
    }
   
    WavHeader header;
    read_wav_header(ptr, header, false);
    if (header.audio_format != 1) { // PCM only
      printf("Only PCM is supported.\n");
      fclose(ptr);
      exit(1);
    }

    long num_samples, size_of_each_sample;
    compute_wave_sample_sizes(ptr, header, num_samples, size_of_each_sample);
    if(N > num_samples) {
      printf("N must be smaller than the number of samples in the file (N=%zu, samples=%li).\n", N, num_samples);
      fclose(ptr);
      exit(1);
    }

    read_pcm_wav_data(ptr, header, N, size_of_each_sample, x);

    fclose(ptr);
  }
  else {
    // Random input
    for(size_t n=0; n<N; n++) {
      x[n] = complex_rand<double>();
    }
  }

  if(save) {
    std::ofstream input_file;
    input_file.open ("Examples/input.txt");
    for(const auto& x_i : x)
      input_file << x_i << std::endl;
    input_file.close();
    std::cout << "Input signal saved to file." << std::endl;
  }

  std::vector<Cpx<double>> y_ref;
  std::vector<Cpx<double>> y;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> stop;
  std::chrono::microseconds duration;
  std::vector<int> dur_dft;
  std::vector<int> dur_fft;

  std::cout << "Running " << N << "-point radix-" << r << " FFT";
  if(bench)
    std::cout << " " << total_rep << " times";
  std::cout << "." << std::endl;

  for(size_t rep=0; rep<total_rep; rep++) {
    // Run reference function
    y_ref = x;
    Bfly<double> dft(N); // radix-N butterfly = N-point DFT
    if(bench)
      start = std::chrono::high_resolution_clock::now();
    dft.run(y_ref.data(), 0, 1);
    if(bench) {
      stop = std::chrono::high_resolution_clock::now();
      duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      dur_dft.push_back(duration.count());
    }
  
    // Run FFT
    y = x;
    if(bench)
      start = std::chrono::high_resolution_clock::now();
    fft<double>(y.data(), N, r);
    reverse_reorder(y, N, r); // re-order output
    if(bench) {
      stop = std::chrono::high_resolution_clock::now();
      duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      dur_fft.push_back(duration.count());
    }
  }

  // Take medians
  if(bench) {
    std::sort(dur_dft.begin(), dur_dft.end()); 
    std::cout << "DFT: " << dur_dft[total_rep/2-1] << " us." << std::endl;
    std::sort(dur_fft.begin(), dur_fft.end()); 
    std::cout << "FFT: " << dur_fft[total_rep/2-1] << " us." << std::endl;
  }

  // Check result
  double delta = get_delta<double>();
  for(size_t i=0; i<N; i++)
    ASSERT(y_ref[i], y[i], delta);

  if(save) {
    std::ofstream output_file;
    output_file.open ("Examples/output.txt");
    for(const auto& y_i : y)
      output_file << y_i << std::endl;
    output_file.close();
    std::cout << "Output signal saved to file." << std::endl;
  }

  std::cout << "Done." << std::endl;

  return 0;
}
