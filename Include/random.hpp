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
