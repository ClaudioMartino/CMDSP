#include <vector>
#include <cmath>
#if defined(BENCH)
#include <chrono>
#endif
#if defined(SAVE)
#include <fstream>
#endif

#include "complex.hpp"
#include "assert.hpp"
#include "random.hpp"

#define PI 3.14159265358979323846

template <typename T>
Cpx<T> get_twiddle(size_t N, double kn) {
  // W_N^(k n) = exp((-j 2 PI k n) / N)
  T r =  cos(2.0 * PI * kn / N);
  T i = -sin(2.0 * PI * kn / N);
  return {r, i};
}

template <typename T>
void ref_dft(const std::vector<Cpx<T>> x, std::vector<Cpx<T>>& y, const size_t N) {
  // X[k] = sum_n x[n] W_N^(kn)
  for(size_t k=0; k<N; k++) {
    Cpx<T> y_k = {0, 0};
    for(size_t n=0; n<N; n++) {
      Cpx<T> w_kn = get_twiddle<T>(N, k*n);
      y_k += x[n] * w_kn;
    }
    y.push_back(y_k);
  }
}

template <typename T>
void reverse_reorder(std::vector<Cpx<T>>& x, size_t N, size_t r) {
  Cpx<T> tmp;
  for(size_t idx_src=1; idx_src<N-1; idx_src++) { // 1st and last never change
    size_t i=1; size_t j=N/r;
    size_t idx_dst=0;
    for (; i<N; i = i * r, j = j / r) {
      idx_dst += j * ( int(idx_src / i) % r );
    }
    // Swap
    if(idx_dst > idx_src) { 
      tmp = x[idx_src];
      x[idx_src] = x[idx_dst];
      x[idx_dst] = tmp;
    }
  }
}

template <typename T>
struct Bfly {
  virtual ~Bfly() = default;
  virtual void run(Cpx<T>* data, size_t idx0, size_t step) = 0;
};

template <typename T>
struct Bfly2 : Bfly<T> {
  void run(Cpx<T>* data, size_t idx0, size_t step) override {
    size_t idx1 = idx0 + step;
  
    Cpx<T> out0 = data[idx0] + data[idx1];
    Cpx<T> out1 = data[idx0] - data[idx1];
  
    data[idx0] = out0;
    data[idx1] = out1;
  };
};

template <typename T>
struct Bfly4 : Bfly<T> {
  void run(Cpx<T>* data, size_t idx0, size_t step) override {
    size_t idx1 = idx0 + step;
    size_t idx2 = idx0 + 2*step;
    size_t idx3 = idx0 + 3*step;

    Cpx<T> out0 = data[idx0] + data[idx1] + data[idx2] + data[idx3];
    Cpx<T> out1 = data[idx0] - data[idx1].rot90() - data[idx2] + data[idx3].rot90();
    Cpx<T> out2 = data[idx0] - data[idx1] + data[idx2] - data[idx3];
    Cpx<T> out3 = data[idx0] + data[idx1].rot90() - data[idx2] - data[idx3].rot90();
 
    data[idx0] = out0;
    data[idx1] = out1;
    data[idx2] = out2;
    data[idx3] = out3;
  };
};

template <typename T>
std::unique_ptr<Bfly<T>> get_butterfly(size_t size) {
  if(size == 4) {
    return std::unique_ptr<Bfly4<T>>(new Bfly4<T>);
  }
  else { // default: radix-2
    return std::unique_ptr<Bfly2<T>>(new Bfly2<T>);
  }
}

template <typename T>
void fft(Cpx<T>* data, size_t N, size_t r) {
  std::unique_ptr<Bfly<T>> b_ptr = get_butterfly<T>(r);

  for(size_t s=1; s<N; s = s*r) {
    // Radix 2:
    //   Stage #1: N2=1 set of N1=N/2 bfly2
    //   Stage #2: N2=2 sets of N1=N/4 bfly2
    // Radix 4:
    //   Stage #1: N2=1 set of N1=N/4 bfly4
    //   Stage #2: N2=4 set of N1=N/16 bfly4
    // Radix R:
    //   Stage #1: N2=1 set of N1=N/R bfly4
    //   Stage #2: N2=R set of N1=N/(RR) bfly4
    //   Stage #S: N2=R^(S-1) set of N1=N/(R^3) bfly4
    size_t N2 = s;
    size_t N1 = N / (r*s);

    for(size_t n2=0; n2<N2; n2++) { // N2 sets...
      for(size_t n1=0; n1<N1; n1++) { // ...of N1 butterflies
        size_t idx0 = n1 + n2*N1*r;

        b_ptr->run(data, idx0, N1);

        if(s != N/r) { // Skip twiddle multiplication in last stage
          for(size_t i=1; i<r; i++) { // Skip mult. by one
            Cpx<T> tw = get_twiddle<T>(N, n1*i*s);
            data[idx0 + i*N1] *= tw;
          }
        }
      }
    }
  }
}

int main(int argc, char** argv) {

  // Input signal
  size_t N = 1024;
  if(argc >= 2)
    N = atoi(argv[1]);

  std::vector<Cpx<double>> x;
  for(size_t n=0; n<N; n++) {
    Cpx<double> c = complex_rand<double>();
    x.push_back(c);
  }

#if defined(SAVE)
  std::ofstream input_file;
  input_file.open ("Examples/input.txt");
  for(const auto& x_i : x)
    input_file << x_i << std::endl;
  input_file.close();
#endif

  // Radix size
  size_t r = 2;
  if(argc >= 3)
    r = atoi(argv[2]);

  // Print message to user
  std::cout << "Running " << N << "-point radix-" << r << " FFT." << std::endl;

  // Run reference function
  std::vector<Cpx<double>> y_ref;
#if defined(BENCH)
  auto start = std::chrono::high_resolution_clock::now();
#endif
  ref_dft<double>(x, y_ref, N);
#if defined(BENCH)
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "DFT: " << duration.count() << " us" << std::endl;
#endif

  // Run FFT
#if defined(BENCH)
  start = std::chrono::high_resolution_clock::now();
#endif
  fft<double>(x.data(), N, r); // in-place FFT
  reverse_reorder(x, N, r); // re-order output
#if defined(BENCH)
  stop = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "FFT: " << duration.count() << " us" << std::endl;
#endif

  // Check result
  double delta = get_delta<double>();
  for(size_t i=0; i<N; i++)
    ASSERT(y_ref[i], x[i], delta);

#if defined(SAVE)
  std::ofstream output_file;
  output_file.open ("Examples/output.txt");
  for(const auto& x_i : x)
    output_file << x_i << std::endl;
  output_file.close();
#endif

  std::cout << "Done." << std::endl;

  return 0;
}
