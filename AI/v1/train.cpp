#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <graphics.h>
#include <thread>
using namespace Eigen;
//#include <cmath>
using namespace std;
using uint = unsigned int;
using usht = unsigned short;
const float esp = 0.00001f;

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
inline float notZero(float x) {
  static uint cnt = 0;
  if(-esp < x && x < esp) cout << "save from dividing zero (" << ++cnt << ")\n";
  return x > esp ? x : x < -esp ? x : x >= 0 ? esp : -esp;
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
    // return x;
    return x / sqrt(1+x*x);
  }
  float derivation(float x) {
    // return 1;
    return 1 / (1+x*x) / sqrt(1+x*x);
  }
  AI() {
    for(int i=0; i<depth-1; i++) {
      mats[i] = MatrixXf(layerSizes[i+1], layerSizes[i]);
      for(uint j=0; j<layerSizes[i+1]; j++) {
        for(uint k=0; k<layerSizes[i]; k++) {
          mats[i](j, k) = random1() * 2;
        }
      }
    }
  }
  MatrixXf ans(const MatrixXf &in) {
    MatrixXf answer(layerSizes[depth-1], 1);
    int maxId = 0;
    float max = -10000000.0f;
    for(int i = 0; i < layerSizes[0]; ++i) {
      if(max < in(i,0)) {
        max = in(i,0);
        maxId = i;
      }
    }
    for(int i = 0; i < layerSizes[0]; ++i) {
      answer(i, 0) = (i == maxId) ? 1.0f : 0.0f;
    }
    return answer;
  }
  MatrixXf exec(const MatrixXf &in) {
    MatrixXf layers[depth];
    layers[0] = in;
    for(int i = 1; i < depth; ++i) {
      layers[i] = mats[i-1] * layers[i-1];
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
      layers[i] = mats[i-1] * layers[i-1];
      for(int j = 0; j < layerSizes[i]; ++j) {
        layers[i](j, 0) = activeFunc(layers[i](j, 0));
      }
    }
    MatrixXf answer(layerSizes[depth-1], 1);
    {
      int maxId = 0;
      float max = layers[0](0, 0);
      for(int i = 1; i < layerSizes[0]; ++i) {
        if(layers[0](i, 0) > max) {
          max = layers[0](i, 0);
          maxId = i;
        }
      }
      for(int i = 0; i < layerSizes[0]; ++i) {
        answer(i, 0) = (i == maxId) ? 1.0f : 0.0f;
      }
    }
    float deff = (layers[depth-1]-answer).squaredNorm();
    return deff;
  }
  float train(float alpha = 0.00001) {
    static constexpr int amount = 50;
    MatrixXf samples[amount][depth];
    // for(int i=0; i<depth; ++i) {
    //   layers[i] = MatrixXf(layerSizes[i], 1);
    // }
    for(int index=0; index<amount; index++) {
      auto &layers = samples[index];
      layers[0] = MatrixXf(layerSizes[0], 1);
      for(int i = 0; i < layerSizes[0]; ++i) {
        layers[0](i, 0) = random01();
      }
      for(int i = 0; i < depth-1; ++i) {
        layers[i+1] = mats[i] * layers[i];
        for(int j = 0; j < layerSizes[i+1]; ++j) {
          layers[i+1](j, 0) = activeFunc(layers[i+1](j, 0));
        }
      }
    }
    MatrixXf derivation(layerSizes[depth-1], 1);
    for(int index=0; index<amount; index++) {
      const auto &layers = samples[index];
      MatrixXf answer(layerSizes[0], 1);
      int maxId = 0;
      float max = layers[0](0, 0);
      for(int i = 1; i < layerSizes[0]; ++i) {
        if(layers[0](i, 0) > max) {
          max = layers[0](i, 0);
          maxId = i;
        }
      }
      for(int i = 0; i < layerSizes[0]; ++i) {
        answer(i, 0) = (i == maxId) ? 1.0f : 0.0f;
      }
      derivation += layers[depth-1] - answer;
    }
    derivation /= amount;
    //cout << derivation.squaredNorm() << "\n";
    const float deff = derivation.squaredNorm();
    for(int i = depth-2; i >= 0; --i) {
      const MatrixXf der = derivation; //size[i+1]
      derivation.resize(layerSizes[i], 1);
      //derivation.Zero();
      for(int j=0; j < layerSizes[i]; ++j) {
        derivation(j, 0) = 0;
      }
      for(int index=0; index<amount; index++) {
        auto &layers = samples[index];
        MatrixXf temp(layerSizes[i], 1);
        for(int j=0; j<layerSizes[i]; j++) {
          for(int k=0; k<layerSizes[i+1]; k++) {
            temp(j,0) += - 2 * (der(k,0) - layers[i+1](k,0)) * this->derivation(layers[i+1](k,0));
          }
          derivation(j,0) -= deff / temp(j,0);
        }
        // for(int j = 0; j < layerSizes[i]; ++j) {
        //   derivation(j, 0) += - der.col(0).dot(mats[i].col(j)) / mats[i].col(j).squaredNorm();
        // }
      }
      derivation /= amount;
      for(int index=0; index<amount; index++) {
        auto &layers = samples[index];
        for(int j=0; j < layerSizes[i]; ++j) {
          for(int k=0; k < layerSizes[i+1]; ++k) {
            // mats[i].col(j) -= der / notZero(layers[i](j, 0)) * alpha;
            mats[i](k, j) -= der(k,0) / notZero(layers[i](j,0)) / this->derivation(layers[i+1](k,0)) * alpha;
          }
        }
      }
    }
    return deff;
  }
};

void drawLineChart(int x, int y, int w, int h, const float *data, int n, int offset, int space) {
  float maxVal = data[0], minVal = data[0];
  for(int i=offset+1, id=0; i!=offset; i++, id++) {
    if(i == n) i = -1;
    const float val = data[i];
    if(val > maxVal) maxVal = val;
    if(val < minVal) minVal = val;
  }
  // const int n = data.size();
  ege::ege_point points[n];
  for(int i=offset+1, id=0; i!=offset; i++, id++) {
    if(i == n) i = -1;
    ege::ege_point p;
    p.x = w + (id-n) * space;
    p.y = h - (data[i] - minVal) / (maxVal - minVal) * h;
    points[x] = p;
  }
  ege::PIMAGE img = ege::newimage(w, h);
  ege::setbkcolor(ege::GRAY, img);
  //ege::setfillcolor(ege::GRAY);
  //ege::fillrect(x-padding, y+padding, x+w+padding, y+h+padding);
  ege::setcolor(ege::WHITE);
  for(size_t i = 1; i < n; ++i) {
    ege::line(points[i-1].x, points[i-1].y, points[i].x, points[i].y, img);
  }
  ege::putimage(x, y, img);
  ege::delimage(img);
}

int main() {
  // thread trainning_thread([](){
  //   ;
  // });
  // trainning_thread.join();
  ege::initgraph(640, 480);
  ege::setcolor(ege::WHITE);
  ege::setbkcolor(ege::BLACK);
  ege::setfillcolor(ege::GRAY);
  ege::cleardevice();
  // freopen("console.txt", "w", stdout);
  // fstream aiData("ai.dat", ios::in | ios::out | ios::trunc);
  auto &aiData = cout;
  // aiData << 123 << "\naabc\n";
  // aiData << "AI data:\n";
  AI ai;
  srand(time(nullptr));
  // vector<float> graph;
train:
  float minDeff = 1000000.0f;
  {
    int depth;
    cin >> depth;
    if(depth) {
      int layerSizes[depth];
      for(int i=0; i<depth; i++) {
        cin >> layerSizes[i];
      }
      for(int i=0; i<depth-1; i++) {
        for(int j=0; j<layerSizes[i]; j++) {
          for(int k=0; k<layerSizes[i+1]; k++) {
            cin >> ai.mats[i](k,j);
          }
        }
      }
      minDeff = -1;
    }
  }
  float graph[101] = {0};
  int offset = 0;
  for(int i = 0; minDeff>=0.0001f; ++i) {
    const float deff = ai.train(0.00002);
    if(deff < minDeff) {
      minDeff = deff;
      printf("%d %1.5f\n", i, minDeff);
      ege::xyprintf(20, 10, "now min: %1.5f(%d)", minDeff, i);
      // if(minDeff < 0.001f) {
      //   break;
      // }
    }
    if(!(i & 0xff)) {
      offset++;
      if(offset == 100) offset = 0;
      graph[offset] = deff;
      // drawLineChart(20, 50, 600, 400, graph, 100, offset, 6);
    }
    ege::delay_ms(0);
  }
  if(minDeff > 0) {
    aiData << AI::depth << " ";
    for(int i=0; i<AI::depth; i++) {
      aiData << ai.layerSizes[i] << " ";
    }
    for(int i=0; i<AI::depth-1; i++) {
      // aiData << ai.mats[i];
      for(int j=0; j<ai.layerSizes[i]; j++) {
        for(int k=0; k<ai.layerSizes[i+1]; k++) {
          aiData << ai.mats[i](k,j) << " ";
        }
      }
    }
  }
  while(true) {
    MatrixXf in(9, 1);
    for(int i = 0; i < 9; ++i) {
      cin >> in(i, 0);
      if(in(0,0) < 0) {
        break;
      }
    }
    bool _continue = false;
    switch(int(-in(0,0))) {
      case 1:
        exit(0);
      case 2:
        goto train;
      case 3: {
        // float minDeff = 1000000.0f;
        // MatrixXf best = MatrixXf::Zero(9, 1);
        // for(int i=0; i<100000; i++) {
          in = MatrixXf::Random(9,1);
          // const float deff = ai.deff(in);
          // if(deff < minDeff) {
          //   minDeff = deff;
          //   best = in;
          // }
        // }
        // cout << deff << "\n";
      }
      // _continue = 1;
      default:;
    }
    // if(_continue) continue;
    {
      MatrixXf res = ai.exec(in), ans = ai.ans(in);
      for(int i=0; i<ai.layerSizes[0]; i++) {
        printf("%s%02.9f %s%02.9f %.0f\n", in(i,0)>=0?" ":"", in(i,0), res(i,0)>=0?" ":"", res(i,0), ans(i,0));
      }
      printf("%.16f\n", ai.deff(in));
    }
  }
  return 0;
}