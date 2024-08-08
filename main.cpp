#include "complex.hpp"

#define ASSERT(test, expected, delta) { \
  if(!((test - expected).abs() <= delta)){ \
    std::cerr << "Assert failed: " << test << " (" << #test << ") != " << expected << " (" << #expected << ")" << std::endl; \
  } \
}

template <typename T>
auto get_delta(){
  if constexpr (std::is_same_v<T, float>) return 1e-3;
  else if constexpr (std::is_same_v<T, double>) return 1e-10;
  else return 0; // perfect equality
}

template <typename T>
T real_rand() {
  // Generate a random value between 10 and 100
  T r = 10 + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / 90));
  return r;
}

template <typename T>
Cpx<T> complex_rand() {
  Cpx<T> c = { real_rand<T>(), real_rand<T>() };
  return c;
}

template <typename T>
void run_tests(size_t tot) {
  // Get delta
  T delta = get_delta<T>();

  for(size_t i=0; i<tot; i++) {
    // Generate random inputs
    Cpx<T> c1 = complex_rand<T>();
    Cpx<T> c2 = complex_rand<T>();

    std::cout << "[#" << i << "] " << c1 << ", " << c2 << std::endl;

    // Compute operations
    Cpx<T> sum    = c1 + c2;
    Cpx<T> diff_1 = c1 - c2;
    Cpx<T> diff_2 = c2 - c1;
    Cpx<T> prod   = c1 * c2;
    Cpx<T> div_1  = c1 / c2;
    Cpx<T> div_2  = c2 / c1;

    // Check results
    ASSERT((sum - c2), c1, delta);
    ASSERT((sum - c1), c2, delta);
    ASSERT((diff_1 + c2), c1, delta);
    ASSERT((diff_2 + c1), c2, delta);
    ASSERT((prod / c2), c1, delta); 
    ASSERT((prod / c1), c2, delta); 
    if constexpr (!(std::is_same_v<T, int>) && !(std::is_same_v<T, unsigned int>)) { // skip integer divisions
      ASSERT((div_1 * c2), c1, delta);
      ASSERT((div_2 * c1), c2, delta);
    }
  }
}

int main() {
  const size_t tot = 100;

  std::cout << "== Tests float ==" << std::endl;
  run_tests<float>(tot);

  std::cout << "== Tests double ==" << std::endl;
  run_tests<double>(tot);

  std::cout << "== Tests int ==" << std::endl;
  run_tests<int>(tot);

  std::cout << "== Tests unsigned int ==" << std::endl;
  run_tests<unsigned int>(tot);

  return 0;
}
