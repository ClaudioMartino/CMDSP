#include "fft.hpp"
#include "wav.hpp"
#include "window.hpp"

int main(int argc, char** argv) {
  // Default values
  size_t N = 1024;
  size_t r = 2;
  bool save = false;
  bool bench = false;
  size_t total_rep = 1;
  bool read_from_file = false;
  char* filename = nullptr;
  bool window = false;

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:r:f:sbhw")) {
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
      case 'w':
        window = true;
        continue;
      case 'h':
      default :
        printf("Usage: fft [-n FFT-size] [-r radix-size] [-f file.wav] [-w] [-s] [-b]\n");
        return 0;
        break;
      case -1:
        break;
    }
    break;
  }

  // Check that N is a power of r
  size_t pow = 1;
  while (pow < N)
    pow *= r;

  if (pow != N) {
    std::cout << "N = " << N << " is not a power of r = " << r << std::endl;
    exit(1);
  }

  // Input signal
  std::vector<Cpx<double>> x(N);

  if(read_from_file) {
    // Read .wav file
    std::ifstream fs(filename, std::ios::binary);
    if(fs.is_open()) {
      WavHeader header;
      signal_from_wav_file(fs, header, x, false);
      fs.close();
    }
    else {
      std::cout << "Cannot open " << filename << std::endl;
      exit(1);
    }
  }
  else {
    // Random input
    for(size_t n=0; n<N; n++) {
      x[n] = complex_rand<double>();
    }
  }

  if(window) {
    // Hann window
    hann_window(x);
  }

  if(save) {
    // Save input signal to file
    std::ofstream input_file;
    input_file.open ("Examples/time.txt");
    for(const auto& x_i : x)
      input_file << x_i << std::endl;
    input_file.close();
    std::cout << "Input signal saved to file." << std::endl;
  }

  std::cout << "Running " << N << "-point radix-" << r << " FFT";
  if(bench)
    std::cout << " " << total_rep << " times";
  std::cout << "." << std::endl;

  std::vector<Cpx<double>> y(N);

  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> stop;
  std::chrono::microseconds duration;
  std::vector<int> dur_fft;

  // Run FFT
  for(size_t rep=0; rep<total_rep; rep++) {
    y = x;
    if(bench)
      start = std::chrono::high_resolution_clock::now();

    fft<double>(y.data(), N, r, false);
    reverse_reorder(y, N, r); // re-order output

    if(bench) {
      stop = std::chrono::high_resolution_clock::now();

      duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      dur_fft.push_back(duration.count());
    }
  }

  if(bench) {
    // Take medians
    std::sort(dur_fft.begin(), dur_fft.end()); 
    std::cout << "Duration: " << dur_fft[total_rep/2-1] << " us." << std::endl;
  }

  if(save) {
    // Save output signal
    std::ofstream output_file;
    output_file.open ("Examples/freq.txt");
    for(const auto& y_i : y)
      output_file << y_i << std::endl;
    output_file.close();
    std::cout << "Output signal saved to file." << std::endl;
  }

  return 0;
}
