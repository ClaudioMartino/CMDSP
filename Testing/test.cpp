#include "complex.hpp"
#include "assert.hpp"
#include "random.hpp"

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

    T c1_real = c1.real();
    T c1_imag = c1.imag();
    Cpx<T> c1_conj = c1.conj();
    Cpx<T> c1_r90 = c1.rot90();
    Cpx<T> c1_r180 = c1.rot180();
    Cpx<T> c1_r270 = c1.rot270();

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

    ASSERT_REAL(c1_real, real(c1), 0);
    ASSERT_REAL(c1_imag, imag(c1), 0);
    ASSERT(c1_conj, conj(c1), 0);
    ASSERT_REAL(real(c1_conj), real(c1), 0);
    ASSERT_REAL(imag(c1_conj), -imag(c1), 0);
    ASSERT(c1_r90, rot90(c1), 0);
    ASSERT_REAL(real(c1_r90), -imag(c1), 0);
    ASSERT_REAL(imag(c1_r90), real(c1), 0);
    ASSERT(c1_r180, rot180(c1), 0);
    ASSERT_REAL(real(c1_r180), -real(c1), 0);
    ASSERT_REAL(imag(c1_r180), -imag(c1), 0);
    ASSERT(c1_r90.rot90(), c1_r180, 0);
    ASSERT(c1_r270, rot270(c1), 0);
    ASSERT_REAL(real(c1_r270), imag(c1), 0);
    ASSERT_REAL(imag(c1_r270), -real(c1), 0);
    ASSERT(c1_r180.rot90(), c1_r270, 0);
    ASSERT(c1_r90.rot180(), c1_r270, 0);
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
