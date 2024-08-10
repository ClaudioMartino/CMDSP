#define ASSERT(test, expected, delta) { \
  if(!((test - expected).abs() <= delta)){ \
    std::cerr << "Assert failed: " << test << " (" << #test << ") != " << expected << " (" << #expected << ")" << std::endl; \
  } \
}

template <typename T>
auto get_delta(){
  if constexpr (std::is_same_v<T, float>) return 1e-3;
  else if constexpr (std::is_same_v<T, double>) return 1e-6;
  else return 0; // perfect equality
}
