#include <cmath>
#include <vector>

#include "constants.hpp"

template <typename T>
void dctII(std::vector<T> x, std::vector<T>& y) {
  size_t N = x.size();
  for(size_t k=0; k<N; k++) {
    y[k] = 0;
    for(size_t n=0; n<N; n++) {
      y[k] = x[n] * cos(PI / N * (n + 0.5) * k);
    }
  }
}

template <typename T>
void dctII_2D(std::vector<std::vector<T>> x, std::vector<std::vector<T>>& y) {
  size_t w = x[0].size(); 
  size_t h = x.size(); 

  // Rows
  for(size_t i=0; i<h; i++) {
    dctII<T>(x[i], y[i]);
  }

  // Columns
  std::vector<T> x_sub(h);
  std::vector<T> y_sub(h);
  for(size_t i=0; i<w; i++) {
    // Select one column
    for(size_t k=0; k<h; k++)
      x_sub[k] = y[k][i];

    dctII(x_sub, y_sub);

    for(size_t k=0; k<h; k++)
      y[k][i] = y_sub[k];
  }
}
