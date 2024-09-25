#include <cstdio>
#include <cmath>
#include <iostream>

template <typename T>
struct Cpx {

  // Constructor
  Cpx(T real = 0.0, T imag = 0.0)
    : r { real }, i { imag } {
  }

  // Operators
  Cpx operator + (const Cpx& a) const {
    return {a.r + this->r, a.i + this->i};
  }

  Cpx operator += (const Cpx& a) {
    *this = *this + a;
    return *this;
  }

  Cpx operator - (const Cpx& a) const {
    return {this->r - a.r, this->i - a.i};
  }

  Cpx operator -= (const Cpx& a) {
    *this = *this - a;
    return *this;
  }

  Cpx operator * (const Cpx& a) const {
    T r = a.r * this->r - a.i * this->i;
    T i = a.r * this->i + a.i * this->r;
    return {r, i};
  }

  Cpx operator *= (const Cpx& a) {
    *this = *this * a;
    return *this;
  }

  Cpx operator / (const Cpx& a) const {
    T den = (a.r * a.r + a.i * a.i);
    T r = (this->r * a.r + this->i * a.i) / den;
    T i = (this->i * a.r - this->r * a.i) / den;
    return {r, i};
  }

  Cpx operator /= (const Cpx& a) {
    *this = *this / a;
    return *this;
  }

  bool operator == (const Cpx& a) const {
    return (a.r == this->r) && (a.i == this->i);
  }

  bool operator <= (const Cpx& a) const {
    T this_abs  = this->abs();
    T other_abs = a.abs();
    return (this_abs < other_abs);
  }

  friend std::ostream& operator <<( std::ostream& out, const Cpx& a) {
    return out << a.r << " + i" << a.i ;
  }

  // Methods (member functions)
  T real() const {
    return r;
  }

  T imag() const {
    return i;
  }

  T abs_sq() const {
    return (r * r + i * i);
  }

  T abs() const {
    return sqrt(this->abs_sq());
  }

  Cpx conj() const {
    return {r, -i};
  }

  Cpx rot90() const {
    return{-i, r};
  }

  Cpx rot180() const {
    return{-r, -i};
  }

  Cpx rot270() const {
    return{i, -r};
  }

  // Attributes
  private:
    T r;
    T i;
};

// Non member functions
template <typename T>
T real(Cpx<T> a) {
  return a.real();
}

template <typename T>
T imag(Cpx<T> a) {
  return a.imag();
}

template <typename T>
T abs_sq(Cpx<T> a) {
  return a.abs_sq();
}

template <typename T>
T abs(Cpx<T> a) {
  return a.abs();
}

template <typename T>
Cpx<T> conj(Cpx<T> a) {
  return a.conj();
}

template <typename T>
Cpx<T> rot90(Cpx<T> a) {
  return a.rot90();
}

template <typename T>
Cpx<T> rot180(Cpx<T> a) {
  return a.rot180();
}

template <typename T>
Cpx<T> rot270(Cpx<T> a) {
  return a.rot270();
}
