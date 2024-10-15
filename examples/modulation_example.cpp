#include "fft.hpp"
#include "wav.hpp"
#include "constants.hpp"

int main(int argc, char** argv) {
  // Default values
  size_t N = 1024;
  size_t r = 2;
  bool save = true;
  bool read_from_file = false;
  char* filename = nullptr;
  bool modulation = false;
  double fc = 0;
  std::string carrier("exp");

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:f:m:c:h")) {
      case 'n':
        N = atoi(optarg);
        continue;
      case 'm':
        modulation = true;
        fc = atof(optarg);
        continue;
      case 'f':
        read_from_file = true;
        filename = optarg;
        continue;
      case 'c':
        carrier = optarg;
        continue;

      case 'h':
      default :
        printf("Usage: modulation -f file.wav -m mod-freq [-n FFT-size] [-c carrier]\n");
        return 0;
        break;
      case -1:
        break;
    }
    break;
  }
     
  // Check options
  if(!modulation) {
    std::cout << "-m option is mandatory." << std::endl;
    exit(1);
  }

  if(!read_from_file) {
    std::cout << "-f option is mandatory." << std::endl;
    exit(1);
  }

  if(carrier.compare("exp") != 0 && carrier.compare("cos") != 0) {
    std::cout << "Invalid carrier type (" << carrier << ")." << std::endl;
    exit(1);
  }

  size_t pow = 1;
  while (pow < N)
    pow *= r;

  if (pow != N) {
    std::cout << "N = " << N << " is not a power of r = " << r << std::endl;
    exit(1);
  }

  // Input signal
  std::vector<Cpx<double>> x(N);
  WavHeader header;
  std::ifstream fs(filename, std::ios::binary);
  if(fs.is_open()) {
    signal_from_wav_file(fs, header, x, false);
    fs.close();
  }
  else {
    std::cout << "Cannot open " << filename << std::endl;
    exit(1);
  }

  // Modulation
  int rate = header.sample_rate;
  std::cout << "Input signal modulated with fc = " << fc << " Hz." << std::endl;
  int phase = 0;
  for(size_t n = 0; n < N; n++) {
    double c_n_r = cos(2.0 * PI * fc * n / rate);
    double c_n_i;
    if(carrier.compare("exp") == 0) {
      c_n_i = sin(2.0 * PI * fc * n / rate);
    }
    else if(carrier.compare("cos") == 0) {
      c_n_i = 0;
    }
    Cpx<double> c_n = {c_n_r, c_n_i};
    x[n] *= c_n;
  }

  // Save .wav
  std::ofstream fso("modulated.wav", std::ios::binary);
  write_wav_header(fso, header);
  long size_of_each_sample = (header.num_channels * header.bits_per_sample) / 8;
  write_pcm_wav_data(fso, header, N, size_of_each_sample, x);
  fso.close();

  // Save time signal
  std::ofstream input_file;
  input_file.open ("tools/time.txt");
  for(const auto& x_i : x)
    input_file << x_i << std::endl;
  input_file.close();

  // Run FFT
  std::cout << "Running " << N << "-point radix-" << r << " FFT." << std::endl;
  std::vector<Cpx<double>> y = x;
  fft<double>(y.data(), N, r, false);
  reverse_reorder(y, N, r); // re-order output

  // Save fft signal
  std::ofstream output_file;
  output_file.open ("tools/freq.txt");
  for(const auto& y_i : y)
    output_file << y_i << std::endl;
  output_file.close();

  return 0;
}
