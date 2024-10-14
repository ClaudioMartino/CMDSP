#include "window.hpp"

// Thanks to https://en.wikipedia.org/wiki/Window_function

void cosine_sum_window(std::vector<Cpx<double>>& x, std::vector<double> a)
{
  size_t N = x.size();
  size_t K = a.size();

  for(size_t n=0; n<N; n++) {
    double hann_n = 0;
    double sign = 1;

    for(size_t k=0; k<K; k++) {
      hann_n += sign * a[k] * cos(2 * PI * k * n / N);
      sign = -sign;
    }

    x[n] *= hann_n;
  }
}

void hamming_window(std::vector<Cpx<double>>& x)
{
  double a0 = 25/46;
  double a1 = 1 - a0;
  std::vector<double> a = {a0, a1};
  cosine_sum_window(x, a);
}

void hann_window(std::vector<Cpx<double>>& x)
{
  double a0 = 1/2;
  double a1 = 1 - a0;
  std::vector<double> a = {a0, a1};
  cosine_sum_window(x, a);
}

void blackman_window(std::vector<Cpx<double>>& x)
{
  double a0 = 7938/18608; // 0.42659
  double a1 = 9240/18608; // 0.49656
  double a2 = 1430/18608; // 0.076849
  std::vector<double> a = {a0, a1, a2};
  cosine_sum_window(x, a);
}

void nuttall_window(std::vector<Cpx<double>>& x)
{
  double a0 = 0.355768;
  double a1 = 0.487396;
  double a2 = 0.144232;
  double a3 = 0.012604;
  std::vector<double> a = {a0, a1, a2, a3};
  cosine_sum_window(x, a);
}
