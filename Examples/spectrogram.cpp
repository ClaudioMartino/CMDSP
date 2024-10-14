#include "fft.hpp"
#include "wav.hpp"
#include "window.hpp"

int main(int argc, char** argv) {
  // Default values
  size_t N = 64;
  size_t r = 2;
  char* filename = nullptr;

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:f:h")) {
      case 'n':
        N = atoi(optarg);
        continue;
      case 'f':
        filename = optarg;
        continue;
      case 'h':
      default :
        printf("Usage: fft [-n FFT-size] -f file.wav\n");
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
  std::vector<Cpx<double>> x;

  std::ifstream fs(filename, std::ios::binary);
  if(fs.is_open()) {
    WavHeader header;
    signal_from_wav_file(fs, header, x, true);
    fs.close();
  }
  else {
    std::cout << "Cannot open " << filename << std::endl;
    exit(1);
  }

  std::cout << "Running " << N << "-point radix-" << r << " FFT";

  std::ofstream output_file;
  output_file.open ("Examples/spectrogram.txt");

  // Run FFT on different frames
  for(size_t i=0; i<x.size() / N ; i++) {
    std::vector<Cpx<double>>::const_iterator first = x.begin() + i*N;
    std::vector<Cpx<double>>::const_iterator last  = x.begin() + (i+1)*N;
    std::vector<Cpx<double>> y(first, last);

    fft<double>(y.data(), N, r, false);
    reverse_reorder(y, N, r); // re-order output

    for(const auto& y_i : y)
      output_file << std::setprecision(5) << y_i.abs() << " ";
    output_file << std::endl;
  }

  output_file.close();

  return 0;
}
