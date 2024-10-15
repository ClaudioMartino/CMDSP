#include "fft.hpp"
#include "wav.hpp"

double energy(std::vector<Cpx<double>> x) {
  double e = 0;
  for(size_t i=0; i<x.size(); i++) {
    e += x[i].abs_sq();
  }
  return e;
}

int main(int argc, char** argv) {
  // Default values
  size_t N = 1024;
  size_t r = 2;
  bool read_from_file = false;
  char* filename = nullptr;
  bool filter = false;
  bool scaling = false;
  double f1 = -1;
  double f2 = -1;

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:f:l:h:s")) {
      case 'n':
        N = atoi(optarg);
        continue;
      case 'l':
        f1 = atof(optarg);
        continue;
      case 'h':
        f2 = atof(optarg);
        continue;
      case 'f':
        read_from_file = true;
        filename = optarg;
        continue;
      case 's':
        scaling = true;
        continue;

      default :
        printf("Usage: filter_example -f file.wav [-n FFT-size] [-l low-freq] [-h high-freq] [-s]\n");
        return 0;
        break;
      case -1:
        break;
    }
    break;
  }

  // Check options
  if(!read_from_file) {
    std::cout << "-f option is mandatory." << std::endl;
    exit(1);
  }

  if(f1 != -1 && f2 != -1) {
    filter = true;
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
    signal_from_wav_file<Cpx<double>>(fs, header, x, false);
    fs.close();
  }
  else {
    std::cout << "Cannot open " << filename << std::endl;
    exit(1);
  }

  int rate = header.sample_rate;

  // Compute cut frequencies samples
  // f = sample * rate / N => sample = f / rate * N
  int s1;
  int s2;
  if(filter) {
    s1 = int(f1 / rate * N);
    s2 = int(f2 / rate * N);
    if(s1 < 0 || s2 >= N/2 - 1) {
      std::cout << "f1 must be bigger than zero and f2 must be smaller than B ";
      std::cout << "(f1 = " << f1 << " Hz, f2 = " << f2 << " Hz, B = " << (float)rate / 2 << " Hz)" << std::endl;
      exit(1);
    }
  }

  std::cout << "Running " << N << "-point radix-" << r << " FFT." << std::endl;

  // Run FFT
  std::vector<Cpx<double>> y = x;
  fft<double>(y.data(), N, r, false);
  reverse_reorder(y, N, r); // re-order output

  // Check Parseval's theorem
  std::cout << "Parseval's theorem: E(x) = " << energy(x) << ", E(y)/N = " << energy(y)/N << "?" << std::endl;

  std::vector<Cpx<double>> inv(N);
  if(filter) {
    // Filter
    std::cout << "Output signal filtered with f1 = " << f1 << " Hz and f2 = " << f2 << " Hz." << std::endl;
    for(size_t k = s1; k < s2; k++) {
      inv[k] = y[k];
      if(k>0)
        inv[N-k] = y[N-k];
    }

    if(scaling) {
      // Scaling
      double energy_ratio = energy(y) / energy(inv);
      std::cout << "Output signal energy scaled by a factor of " << energy_ratio << "." << std::endl;
      for(size_t k=s1; k<s2; k++) {
        inv[k] *= energy_ratio;
        if(k>0)
          inv[N-k] *= energy_ratio;
      }
    }
  }
  else {
    // If the cut frequencies are not defined, do not filter
    inv = y;
  }

  // Save filtered FFT signal
  std::ofstream output_file;
  output_file.open ("tools/freq.txt");
  for(const auto& inv_i : inv)
    output_file << inv_i << std::endl;
  output_file.close();

  // Run inverse FFT
  fft<double>(inv.data(), N, r, true);
  reverse_reorder(inv, N, r);

  // Save .wav file
  std::vector<double> real_inv(N);
  for(size_t i=0; i<N; i++)
    real_inv[i] = real(inv[i]);
  std::ofstream fso("filtered.wav", std::ios::binary);
  write_wav_header(fso, header);
  long size_of_each_sample = (header.num_channels * header.bits_per_sample) / 8;
  write_pcm_wav_data<double>(fso, header, N, size_of_each_sample, real_inv);
  fso.close();
 
  // Save inverse signal
  std::ofstream input_file;
  input_file.open ("tools/time.txt");
  for(const auto& inv_i : inv)
    input_file << inv_i << std::endl;
  input_file.close();

  return 0;
}
