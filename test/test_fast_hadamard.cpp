#include "hadamard.hpp"
#include "assert.hpp"

int main() {
  std::vector<size_t> Nvec = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

  for(size_t i=0; i<Nvec.size(); i++) {
    size_t N = Nvec[i];
  
    std::cout << "[N = " << N << "]" << std::endl;

    // Initialize input
    std::vector<double> x(N);
    for(size_t i=0; i<N; i++) {
      x[i] = real_rand<double>();
    }
  
    // Compute matrix to vector multiplication
    Matrix<double> Hn = walsh_hadamard_matrix(N);
    std::vector<double> y = Hn * x;
  
    // Compute Fast Walsh Hadamard
    fast_walsh_hadamard<double>(x);
  
    // Compare
    double delta = get_delta<double>();
    for(size_t i=0; i<N; i++) {
      ASSERT_REAL(x[i], y[i], delta);
    }
  }

  return 0;
}


