#include <getopt.h>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>

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
struct Bfly {
  Bfly() = default;

  Bfly(size_t r) : size { r } { };

  virtual ~Bfly() = default;

  virtual void run(Cpx<T>* data, size_t idx0, size_t step) {
    std::vector<Cpx<T>> out_v;
    for(size_t r=0; r<size; r++) {
      Cpx<T> out = data[idx0];
      for(size_t rr=1; rr<size; rr++) {
        out += get_twiddle<T>(size, r * rr) * data[idx0 + rr*step];
      }
      out_v.push_back(out);
    }
    for(size_t r=0; r<size; r++) {
      data[idx0 + r*step] = out_v[r];
    }
  };

  private:
    size_t size = 0;
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

    //Cpx<T> out0 = data[idx0] + data[idx1] + data[idx2] + data[idx3];
    //Cpx<T> out1 = data[idx0] - data[idx1].rot90() - data[idx2] + data[idx3].rot90();
    //Cpx<T> out2 = data[idx0] - data[idx1] + data[idx2] - data[idx3];
    //Cpx<T> out3 = data[idx0] + data[idx1].rot90() - data[idx2] - data[idx3].rot90();

    // Compute intermediate values to help the compiler
    Cpx<T> sum02 = data[idx0] + data[idx2];
    Cpx<T> sum13 = data[idx1] + data[idx3];
    Cpx<T> dff02 = data[idx0] - data[idx2];
    Cpx<T> dff13 = data[idx1] - data[idx3];

    Cpx<T> out0 = sum02 + sum13;
    Cpx<T> out1 = dff02 - dff13.rot90();
    Cpx<T> out2 = sum02 - sum13;
    Cpx<T> out3 = dff02 + dff13.rot90();
 
    data[idx0] = out0;
    data[idx1] = out1;
    data[idx2] = out2;
    data[idx3] = out3;
  };
};

template <typename T>
struct Bfly8 : Bfly<T> {
  void run(Cpx<T>* data, size_t idx0, size_t step) override {
    size_t idx1 = idx0 + step;
    size_t idx2 = idx0 + 2*step;
    size_t idx3 = idx0 + 3*step;
    size_t idx4 = idx0 + 4*step;
    size_t idx5 = idx0 + 5*step;
    size_t idx6 = idx0 + 6*step;
    size_t idx7 = idx0 + 7*step;

    Cpx<T> in0 = data[idx0]; 
    Cpx<T> in1 = data[idx1]; 
    Cpx<T> in2 = data[idx2]; 
    Cpx<T> in3 = data[idx3]; 
    Cpx<T> in4 = data[idx4]; 
    Cpx<T> in5 = data[idx5]; 
    Cpx<T> in6 = data[idx6]; 
    Cpx<T> in7 = data[idx7]; 

    Cpx<T> rot45 = {sqrt(2)/2, sqrt(2)/2}; 

    Cpx<T> out0 = in0 + in1 + in2 + in3
                + in4 + in5 + in6 + in7; 
    Cpx<T> out1  = in0;
           out1 += in1.rot270() * rot45;
           out1 += in2.rot270();
           out1 -= in3 * rot45; // += in3.rot180() * rot45;
           out1 -= in4;
           out1 += in5.rot90() * rot45;
           out1 += in6.rot90();
           out1 += in7 * rot45;
    Cpx<T> out2  = in0;
           out2 += in1.rot270();
           out2 -= in2;
           out2 += in3.rot90();
           out2 += in4;
           out2 += in5.rot270();
           out2 -= in6;
           out2 += in7.rot90();
    Cpx<T> out3  = in0;
           out3 -= in1 * rot45; // += in1.rot180() * rot45;
           out3 += in2.rot90();
           out3 += in3.rot270() * rot45;
           out3 -= in4;
           out3 += in5 * rot45;
           out3 += in6.rot270();
           out3 += in7.rot90() * rot45;
    Cpx<T> out4  = in0;
           out4 -= in1;
           out4 += in2;
           out4 -= in3;
           out4 += in4;
           out4 -= in5;
           out4 += in6;
           out4 -= in7;
    Cpx<T> out5  = in0;
           out5 += in1.rot90() * rot45;
           out5 += in2.rot270();
           out5 += in3 * rot45;
           out5 -= in4;
           out5 += in5.rot270() * rot45;
           out5 += in6.rot90();
           out5 -= in7 * rot45; // += in7.rot180() * rot45;
    Cpx<T> out6  = in0;
           out6 += in1.rot90();
           out6 -= in2;
           out6 += in3.rot270();
           out6 += in4;
           out6 += in5.rot90();
           out6 -= in6;
           out6 += in7.rot270();
    Cpx<T> out7  = in0;
           out7 += in1 * rot45;
           out7 += in2.rot90();
           out7 += in3.rot90() * rot45;
           out7 -= in4;
           out7 -= in5 * rot45; // += in5.rot180() * rot45;
           out7 += in6.rot270();
           out7 += in7.rot270() * rot45;

    data[idx0] = out0;
    data[idx1] = out1;
    data[idx2] = out2;
    data[idx3] = out3;
    data[idx4] = out4;
    data[idx5] = out5;
    data[idx6] = out6;
    data[idx7] = out7;
  };
};

template <typename T>
std::unique_ptr<Bfly<T>> get_butterfly(size_t size) {
  if(size == 2) {
    return std::unique_ptr<Bfly2<T>>(new Bfly2<T>);
  }
  else if(size == 4) {
    return std::unique_ptr<Bfly4<T>>(new Bfly4<T>);
  }
  else if(size == 8) {
    return std::unique_ptr<Bfly8<T>>(new Bfly8<T>);
  }
  else {
    return std::unique_ptr<Bfly<T>>(new Bfly<T>(size));
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


