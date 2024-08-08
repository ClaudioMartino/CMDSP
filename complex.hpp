#include <cstdio>
#include <cmath>
#include <iostream>
//#include <complex>

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

  T abs() const {
    return sqrt(r * r + i * i);
  }

  T conj() const {
    return {r, -i};
  }

  // Attributes
  private:
    T r;
    T i;
};
