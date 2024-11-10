#include <iostream>

#include "dct.hpp"

int main() {
  size_t N = 8;
  size_t w = N;
  size_t h = N;

  // 2D signals
  std::vector<std::vector<double>> x(h);
  std::vector<std::vector<double>> y(h);
  for(size_t i=0; i<h; i++) {
    x[i].resize(w);
    y[i].resize(w);
    for(size_t k=0; k<w; k++) {
      x[i][k] = (double)0;
    }
  }

  dctII_2D<double>(x, y);

  for(size_t i=0; i<h; i++) {
    for(size_t k=0; k<w; k++) {
      std::cout << y[i][k] << " ";
    }
    std::cout << std::endl;
  }


  return 0;
}
