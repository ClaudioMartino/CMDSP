#include <getopt.h>

#include "hadamard.hpp"

int main(int argc, char** argv) {
  // Default value
  size_t N = 1024;

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:")) {
      case 'n':
        N = atoi(optarg);
        continue;
      case 'h':
      default :
        printf("Usage: hadamard_example [-n size]\n");
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
    pow *= 2;

  if (pow != N) {
    std::cout << "N = " << N << " is not a power of 2." << std::endl;
    exit(1);
  }

  // Initialize input
  std::vector<double> x(N);
  for(size_t i=0; i<N; i++) {
    x[i] = real_rand<double>();
  }

  // Compute Fast Walsh Hadamard
  fast_walsh_hadamard<double>(x);

  return 0;
}


