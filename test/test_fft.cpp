#include "fft.hpp"
#include "assert.hpp"
#include "random.hpp"

int main() {
  std::vector<size_t> Nvec = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  std::vector<std::vector<size_t>> rvec { 
    {2},          // 8
    {2, 4},       // 16
    {2},          // 32
    {2, 4, 8},    // 64
    {2},          // 128
    {2, 4, 16},   // 256
    {2, 8},       // 512
    {2, 4},       // 1024
    {2},          // 2048
    {2, 4, 8, 16} // 4096
  }; 

  double delta = get_delta<double>();

  for(size_t i=0; i<Nvec.size(); i++) {
    size_t N = Nvec[i];
    std::vector<Cpx<double>> x(N);

    for(size_t j=0; j<rvec[i].size(); j++) {
      size_t r = rvec[i][j];

      std::cout << "[N = " << N << ", R = " << r << "]" << std::endl;

      // Random input signal
        for(size_t n=0; n<N; n++) {
          x[n] = complex_rand<double>();
        }
    
      std::cout << " DFT vs. FFT" << std::endl;

      // Run reference DFT
      std::vector<Cpx<double>> y_ref = x;
      Bfly<double> dft(N); // radix-N butterfly = N-point DFT
      dft.run(y_ref.data(), 0, 1, false);
    
      // Run FFT
      std::vector<Cpx<double>> y = x;
      fft<double>(y.data(), N, r, false);
      reverse_reorder(y, N, r); // re-order output
    
      // Compare FFT and DFT
      for(size_t i=0; i<N; i++)
        ASSERT(y_ref[i], y[i], delta);
    
      std::cout << " FFT-IFFT" << std::endl;

      // Run inverse FFT
      std::vector<Cpx<double>> inv = y;
      fft<double>(inv.data(), N, r, true);
      reverse_reorder(inv, N, r);
    
      // Compare IFFT and input signal
      for(size_t i=0; i<N; i++)
        ASSERT(inv[i], x[i], delta);
    }
  }

  return 0;
}
