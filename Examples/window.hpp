#include <vector>

#include "complex.hpp"
#include "constants.hpp"

void cosine_sum_window(std::vector<Cpx<double>>& x, std::vector<double> a);
void hamming_window(std::vector<Cpx<double>>& x);
void hann_window(std::vector<Cpx<double>>& x);
