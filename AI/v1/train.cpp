#include <iostream>
#include <Eigen/Dense>
using namespace Eigen;
//#include <cmath>
using namespace std;
using uint = unsigned int;
using usht = unsigned short;

inline float random1() {
  return (float(rand())/RAND_MAX*2)-1;
}
inline float random01() {
  return float(rand())/RAND_MAX;
}
template <typename T>
inline auto square(T a) {
  return a*a;
}
inline float clamp(float x, float min, float max) {
  return x<min ? min : x>max ? max : x;
}

/*
template <uint w=0, uint h=0>
class Matrix;
template <>
class MatrixXf {
private:
public:
  uint w=0, h=0;
  float *data=nullptr;
public:
  Matrix() : w(0), h(0), data(nullptr) {}
  Matrix(uint width, uint height) : w(width), h(height) {
    data = new float[w * h];
    for (uint i = 0; i < w * h; ++i) {
      data[i] = 0.0f;
    }
  }
  ~Matrix() {
    delete[] data;
  }
  float& operator()(uint x, uint y) {
    return data[y * w + x];
  }
  const float& operator()(uint x, uint y) const {
    return data[y * w + x];
  }
  // void operator delete[](void* ptr) {
  //   ::operator delete[](ptr);
  // }
  Matrix &operator=(const Matrix &other) {
    if (this != &other) {
      if (w != other.w || h != other.h) {
        throw std::runtime_error("Matrix dimensions do not match for assignment.");
      }
      for (uint i = 0; i < w * h; ++i) {
        data[i] = other.data[i];
      }
    }
    return *this;
  }
  Matrix &operator*=(const Matrix &other) {
    if(w != other.h) {
      throw std::runtime_error("Matrix dimensions do not match for multiplication.");
    }
    Matrix result(w, other.h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < other.h; ++j) {
        result(i, j) = 0;
        for (uint k = 0; k < h; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    *this = result;
    return *this;
  }
  Matrix operator*(const Matrix &other) const {
    if(w != other.h) {
      throw std::runtime_error("Matrix dimensions do not match for multiplication.");
    }
    Matrix result(w, other.h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < other.h; ++j) {
        //result(i, j) = 0;
        for (uint k = 0; k < h; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }
  Matrix &operator+=(const Matrix &other) {
    if(w != other.w || h != other.h) {
      throw std::runtime_error("Matrix dimensions do not match for addition.");
    }
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        (*this)(i, j) += other(i, j);
      }
    }
    return *this;
  }
  Matrix operator+(const Matrix &other) const {
    if(w != other.w || h != other.h) {
      throw std::runtime_error("Matrix dimensions do not match for addition.");
    }
    Matrix result(w, h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = (*this)(i, j) + other(i, j);
      }
    }
    return result;
  }
  Matrix &operator-=(const Matrix &other) {
    if(w != other.w || h != other.h) {
      throw std::runtime_error("Matrix dimensions do not match for subtraction.");
    }
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        (*this)(i, j) -= other(i, j);
      }
    }
    return *this;
  }
  Matrix operator-(const Matrix &other) const {
    if(w != other.w || h != other.h) {
      throw std::runtime_error("Matrix dimensions do not match for subtraction.");
    }
    Matrix result(w, h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = (*this)(i, j) - other(i, j);
      }
    }
    return result;
  }
  Matrix operator-() {
    Matrix result(w, h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = -(*this)(i, j);
      }
    }
    return result;
  }
  Matrix &operator*=(float scalar) {
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        (*this)(i, j) *= scalar;
      }
    }
    return *this;
  }
  Matrix operator*(float scalar) const {
    Matrix result(w, h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = (*this)(i, j) * scalar;
      }
    }
    return result;
  }
  Matrix &operator/=(float scalar) {
    if (scalar == 0) {
      throw std::runtime_error("Division by zero in matrix division.");
    }
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        (*this)(i, j) /= scalar;
      }
    }
    return *this;
  }
  Matrix operator/(float scalar) const {
    if (scalar == 0) {
      throw std::runtime_error("Division by zero in matrix division.");
    }
    Matrix result(w, h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = (*this)(i, j) / scalar;
      }
    }
    return result;
  }
  Matrix transpose() const {
    Matrix result(h, w);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }
  Matrix inverse() const {
    if (w != h) {
      throw std::runtime_error("Matrix must be square for inversion.");
    }
    // Implementing a simple Gauss-Jordan elimination for matrix inversion
    Matrix result(w, h);
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = (i == j) ? 1.0f : 0.0f;
      }
    }
    // Copy original matrix to result
    for (uint i = 0; i < w; ++i) {
      for (uint j = 0; j < h; ++j) {
        result(i, j) = (*this)(i, j);
      }
    }
    // Perform Gauss-Jordan elimination
    for (uint i = 0; i < w; ++i) {
      float pivot = result(i, i);
      if (pivot == 0) {
        throw std::runtime_error("Matrix is singular and cannot be inverted.");
      }
      for (uint j = 0; j < h; ++j) {
        result(i, j) /= pivot;
        result(i, j + h) /= pivot;
      }
      for (uint k = 0; k < w; ++k) {
        if (k != i) {
          float factor = result(k, i);
          for (uint j = 0; j < h * 2; ++j) {
            result(k, j) -= factor * result(i, j);
          }
        }
      }
    }
    return result;
  }
  friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    for (uint i = 0; i < matrix.h; ++i) {
      for (uint j = 0; j < matrix.w; ++j) {
        os << matrix(i, j) << " ";
      }
      os << "\n";
    }
    return os;
  }
  friend std::istream& operator>>(std::istream& is, Matrix& matrix) {
    for (uint i = 0; i < matrix.h; ++i) {
      for (uint j = 0; j < matrix.w; ++j) {
        is >> matrix(i, j);
      }
    }
    return is;
  }
};
*/
// template <uint w>
// using VecHor = Matrix<w, 1>;
// template <uint h>
// using VecVer = Matrix<1, h>;
// template <uint size>
// using Vec = VecHor<size>;

class AI {
public:
  const static uint depth = 4;
  const uint layerSizes[depth] = { 9, 36, 18, 9 };
  MatrixXf mats[depth-1];
  float activeFunc(float x) {
    return x / sqrt(1+x*x);
  }
  float derivation(float x) {
    return 1 / (1+x*x) / sqrt(1+x*x);
  }
  AI() {
    for(int i=0; i<depth-1; i++) {
      mats[i] = MatrixXf(layerSizes[i], layerSizes[i+1]);
      for(uint j=0; j<layerSizes[i]; j++) {
        for(uint k=0; k<layerSizes[i+1]; k++) {
          mats[i](j, k) = random1() * 2;
        }
      }
    }
  }
  MatrixXf exec(const MatrixXf &in) {
    MatrixXf layers[depth];
    layers[0] = in;
    for(int i = 1; i < depth; ++i) {
      layers[i] = layers[i-1] * mats[i-1];
      for(int j = 0; j < layerSizes[i]; ++j) {
        layers[i](j, 0) = activeFunc(layers[i](j, 0));
      }
    }
    return layers[depth-1];
  }
  float deff(const MatrixXf &in) {
    MatrixXf layers[depth];
    layers[0] = in;
    for(int i = 1; i < depth; ++i) {
      layers[i] = layers[i-1] * mats[i-1];
      for(int j = 0; j < layerSizes[i]; ++j) {
        layers[i](j, 0) = activeFunc(layers[i](j, 0));
      }
    }
    MatrixXf answer(layerSizes[depth-1], 1);
    {
      int maxId = 0;
      float max = layers[depth-1](0, 0);
      for(int i = 1; i < layerSizes[depth-1]; ++i) {
        if(layers[depth-1](i, 0) > max) {
          max = layers[depth-1](i, 0);
          maxId = i;
        }
      }
      for(int i = 0; i < layerSizes[depth-1]; ++i) {
        answer(i, 0) = (i == maxId) ? 1.0f : 0.0f;
      }
    }
    float deff = square(layers[depth-1]-answer).sum() / layerSizes[depth-1];
    return deff;
  }
  float train(float alpha = 0.0001) {
    MatrixXf layers[depth];
    // for(int i=0; i<depth; ++i) {
    //   layers[i] = MatrixXf(layerSizes[i], 1);
    // }
    for(int i = 0; i < layerSizes[0]; ++i) {
      layers[0](i, 0) = random01();
    }
    for(int i = 0; i < depth-1; ++i) {
      layers[i+1] = layers[i] * mats[i];
      for(int j = 0; j < layerSizes[i+1]; ++j) {
        layers[i+1](j, 0) = activeFunc(layers[i+1](j, 0));
      }
    }
    MatrixXf answer(layerSizes[depth-1], 1);
    {
      int maxId = 0;
      float max = layers[depth-1](0, 0);
      for(int i = 1; i < layerSizes[depth-1]; ++i) {
        if(layers[depth-1](i, 0) > max) {
          max = layers[depth-1](i, 0);
          maxId = i;
        }
      }
      for(int i = 0; i < layerSizes[depth-1]; ++i) {
        answer(i, 0) = (i == maxId) ? 1.0f : 0.0f;
      }
    }
    MatrixXf derivation(layerSizes[depth-1], 1);
    derivation = layers[depth-1] - answer;
    const float deff = (derivation*derivation).sum() / layerSizes[depth-1];
    for(int i = depth-2; i >= 0; --i) {
      const MatrixXf der = derivation;
      for(int j = 0; j < layerSizes[i+1]; ++j) {
        derivation(j, 0) = - layers[i+1].col(0).dot(mats[i].row(j)) / mats[i].row(j).squaredNorm();
      }
      for(int j = 0; j < layerSizes[i]; ++j) {
        mats[i].row(j) -= der / layers[i](j, 0) * alpha;
      }
    }
    return deff;
  }
};

int main() {
  AI ai;
  for(int i = 0; i < 10000; ++i) {
    cout << ai.train(0.0001) << "\n";
  }
  while(true) {
    MatrixXf in(9, 1);
    for(int i = 0; i < 9; ++i) {
      cin >> in(i, 0);
    }
    if(in(0, 0) < 0) {
      break;
    }
    cout << ai.exec(in) << "\t" << ai.deff(in) << "\n";
  }
  return 0;
}