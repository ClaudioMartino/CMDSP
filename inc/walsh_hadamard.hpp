#include <vector>
#include <cstdio>
#include <cmath>
#include <iostream>

#include "matrix.hpp"
#include "assert.hpp"
#include "random.hpp"

#define INVSQRT2 0.707106781186548

Matrix<double> walsh_hadamard_matrix(size_t N) {
  // H1  
  Matrix<double> H1(2, 2);
  H1.set(0, 0, 1);
  H1.set(0, 1, 1);
  H1.set(1, 0, 1);
  H1.set(1, 1, -1);
  H1 = H1 * INVSQRT2;

  std::vector<Matrix<double>> mat_vector;
  Matrix<double> * Hptr;
  mat_vector.push_back(H1);

  for(size_t i=0; i<log2(N)-1; i++) {
    Hptr = &mat_vector.back();
    mat_vector.push_back(kronecker<double>(H1, *Hptr) );
  }

  return mat_vector.back();
}

template <typename T>
void fast_walsh_hadamard(std::vector<T>& a) {
  size_t N = a.size();
  for(size_t h=1; h<N; h*=2) {
    for(size_t i=0; i<N; i+=2*h) {
      for(size_t j=i; j<i+h; j++) {
         T x = a[j];
         T y = a[j + h];
         a[j] = x + y;
         a[j + h] = x - y;
      }
    }
    for(size_t i=0; i<N; i++) {
      a[i] *= INVSQRT2;
    }
  }
}
