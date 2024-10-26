#include "walsh_hadamard.hpp"

int main() {
  size_t N = 16;

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

  return 0;
}


