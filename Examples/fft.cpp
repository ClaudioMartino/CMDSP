#include <vector>
#include <cmath>
#if defined(BENCH)
#include <chrono>
#endif

#include "complex.hpp"
#include "assert.hpp"
#include "random.hpp"

#define PI 3.14159265358979323846

template <typename T>
Cpx<T> get_twiddle(size_t N, double kn) {
  // W_N^(kn) = exp((-j 2 PI k n) / N)
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
void bit_reverse_reorder(std::vector<Cpx<T>>& x, size_t N) {
  Cpx<T> tmp;
  
  double bits = log2(N);

  for (size_t i=0; i<N; i++) {
    size_t j=0;
    for (size_t k=0; k<bits; k++)
      if (i & (size_t)pow(2, k))
        j += pow(2, bits - k - 1);
  
    if (j>i) {
        tmp = x[i];
        x[i] = x[j];
        x[j] = tmp;
    }
  }
}

template <typename T>
void bfly2(Cpx<T>& in0, Cpx<T>& in1) {
  Cpx<T> out0 = in0 + in1;
  Cpx<T> out1 = in0 - in1;

  in0 = out0;
  in1 = out1;
}

template <typename T>
void radix2_fft(Cpx<T>* data, size_t N) {
  for(size_t s=1; s<N; s = s << 1) {
    // stage #1: N2=1 set of N1=N/2 bfly2
    // stage #2: N2=2 sets of N1=N/4 bfly2
    size_t N2 = s;
    size_t N1 = N / (2*s);

    for(size_t n2=0; n2<N2; n2++) { // N2 sets...
      for(size_t n1=0; n1<N1; n1++) { // ...of N1 butterflies
        size_t idx0 = n1 + n2*N1*2;
        size_t idx1 = n1 + n2*N1*2 + N1;

        bfly2<T>(data[idx0], data[idx1]);

        if(s != N/2) { // Skip butterfly in last stage
          Cpx<T> tw = get_twiddle<T>(N/s, n1);
          data[idx1] *= tw;
        }
      }
    }
  }
}

int main(int argc, char** argv) {

  // Input signal
  size_t N;
  if(argc < 2)
    N = 1024;
  else
    N = atoi(argv[1]);

  std::cout << "Running " << N << "-FFT" << std::endl;

  std::vector<Cpx<double>> x;
  for(size_t n=0; n<N; n++) {
    Cpx<double> c = complex_rand<double>();
    x.push_back(c);
  }

  // Run reference function
  std::vector<Cpx<double>> y_ref;
#if defined(BENCH)
  auto start = std::chrono::high_resolution_clock::now();
#endif
  ref_dft<double>(x, y_ref, N);
#if defined(BENCH)
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << duration.count() << std::endl;
#endif

  // Run FFT
#if defined(BENCH)
  start = std::chrono::high_resolution_clock::now();
#endif
  radix2_fft<double>(x.data(), N);
  bit_reverse_reorder(x, N);
#if defined(BENCH)
  stop = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << duration.count() << std::endl;
#endif

  // Check result
  double delta = get_delta<double>();
  for(size_t i=0; i<N; i++)
    ASSERT(y_ref[i], x[i], delta);

  return 0;
}
