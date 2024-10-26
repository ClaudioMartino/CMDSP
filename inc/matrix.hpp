#include <cstdio>
#include <cstring>
#include <stdexcept>

template <typename T>
struct Matrix {
  // Constructor
  Matrix(size_t rows, size_t cols)
  : rows { rows } , cols { cols } {
    buf = new T[rows*cols];
    buf[0] = 0;
  }

  // Destructor
  ~Matrix() {
    if(buf != nullptr)
      delete[] buf;
  }

  // Copy constructor
  Matrix(const Matrix& other) {
    rows = other.rows;
    cols = other.cols;
    buf = new T[rows*cols];
    memcpy(buf, other.buf, rows*cols*sizeof(T));	
  }

  Matrix& operator=(const Matrix& other) {
    if(this == &other)
      return *this;

    rows = other.rows;
    cols = other.cols;
    if(buf)
      delete[] buf;
    buf = new T[rows*cols];
    memcpy(buf, other.buf, rows*cols*sizeof(T));	
    return *this;
  }

  // Move constructor
  Matrix(Matrix&& other) noexcept {
    rows = other.rows;
    cols = other.cols;
    buf = other.buf;

    other.buf = nullptr;
    other.rows = 0;
    other.cols = 0;
  }

  // Methods
  void print() {
    for(size_t r=0; r<rows; r++) {
      for(size_t c=0; c<cols; c++) {
        std::cout << buf[r*cols + c];
        std::cout << " ";
      }
      std::cout << std::endl;
    }
  }

  T r() {
    return this->rows;
  }

  T c() {
    return this->cols;
  }

  void set(const size_t r, const size_t c, const T val) {
    if(r > this->rows - 1 || c > this->cols -1)
      std::cout << "ERR set " << r << c << std::endl;
    buf[r*cols + c] = val;
  }

  auto get(const size_t r, const size_t c) const {
    if(r > this->rows - 1 || c > this->cols -1)
      std::cout << "ERR get " << r << c << std::endl;
    return buf[r*cols + c];
  }

  // Operators
  Matrix& operator=(Matrix&& other) noexcept {
    if(this == &other) return *this;
    delete[] buf;
    rows = other.rows;
    cols = other.cols;
    buf = new T[rows*cols];
    memcpy(buf, other.buf, rows*cols*sizeof(T));	
    return *this;
  }

  Matrix operator * (const Matrix& a) const {
    // TODO check that this.cols = a.rows
    Matrix res(this->rows, a.cols);

    for(size_t r=0; r<res.rows; r++) {
      for(size_t c=0; c<res.cols; c++) {
        res.set(r, c, 0);
        auto tmp = res.get(r,c);
        for(size_t k=0; k<this->cols; k++) {
          tmp += this->get(r, k) * a.get(k, c);
        }
        res.set(r, c, tmp);
      }
    }
    return res;
  }

  Matrix operator * (const T& a) const {
    Matrix res(this->rows, this->cols);
    for(size_t r=0; r<res.rows; r++) {
      for(size_t c=0; c<res.cols; c++) {
        res.set(r, c, this->get(r, c) * a);
      }
    }
    return res;
  }

  std::vector<T> operator * (const std::vector<T>& a) const {
    // TODO check that this.cols = a.size()
    std::vector<T> res(a.size());

    for(size_t r=0; r<this->rows; r++) {
      res[r] = 0;
      for(size_t c=0; c<this->cols; c++) {
        res[r] += this->get(r, c) * a[c];
      }
    }
    return res;
  }

  // Attributes
  private:
    T * buf;
    size_t rows;
    size_t cols;
};

template <typename T>
Matrix<T> kronecker(Matrix<T>& A, Matrix<T>& B) {
  Matrix<T> res(A.r() * B.r(), A.c() * B.c());

  for(size_t r=0; r<A.r(); r++) {
    for(size_t c=0; c<A.c(); c++) {
      Matrix<T> tmp(B.r(), B.c());
      tmp = B * A.get(r, c);
      // Copy tmp to res
      for(size_t rr=0; rr<B.r(); rr++) {
        for(size_t cc=0; cc<B.c(); cc++) {
          res.set(r*B.r() + rr, c*B.c() + cc, tmp.get(rr, cc));
        }
      }

    }
  }

  return res;
}
