#include <iostream>
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
inline float square(float a) {
  return a*a;
}
inline float clamp(float x, float min, float max) {
  return x<min ? min : x>max ? max : x;
}

class Layer {
private:
  usht iSize, oSize;
  float *w, *b;
public:
  float activeFunc(float x) {
    return clamp(x, -5, 5);
  }
  float derivation(float x) {
    return x>-5 && x<5;
  }
  Layer() : iSize(0), oSize(0) {
    w = new float[0];
    b = new float[0];
  }
  Layer(usht iSize, usht oSize) : iSize(iSize), oSize(oSize) {
    w = new float[iSize*oSize];
    b = new float[oSize];
    for(int i=0; i<iSize*oSize; i++) {
      w[i] = random1() * 5;
    }
    for(int i=0; i<oSize; i++) {
      b[i] = random1() * 5;
    }
  }
  float *calc(float *in) {
    float *out = new float[oSize];
    for(int i=0; i<oSize; i++) {
      out[i] = 0;
    }
    for(int j=0; j<oSize; j++) {
      for(int i=0; i<iSize; i++) {
        out[j] += in[i] * w[i*oSize+j];
      }
      out[j] += b[j];
      out[j] = activeFunc(out[j]);
    }
    return out;
  }
  ~Layer() {
    delete w;
    delete b;
  }
};
class AI {
private:
  Layer layers[3];
  static const usht depth = 3;
public:
  AI() {
    layers[0] = Layer(9, 20);
    layers[1] = Layer(20, 20);
    layers[2] = Layer(20, 9);
  }
  float *calc(float *in) {
    for(int i=0; i<depth; i++) {
      float *out = layers[i].calc(in);
      delete in;
      in = out;
    }
  }
  float reward(float *in) {
    float ans[9];
    int max = in[0], maxId = 0;
    for(int i=1; i<9; i++) {
      if(in[i] > max) {
        max = in[i];
        maxId = i;
      }
    }
    for(int i=0; i<9; i++) {
      ans[i] = i==maxId;
    }
    float *res = calc(in);
    float def = 0;
    for(int i=0; i<9; i++) {
      def += square(res[i] - ans[i]);
    }
    delete res;
    return -def;
  }
  float test() {
    static const usht group = 10;
    float in[9], reward = 0;
    for(int i=0; i<group; i++) {
      for(int j=0; j<9; j++) {
        in[j] = random1();
      }
      reward += this->reward(in);
    }
    return reward / group;
  }
  float train(float k = 0.001) {
    ;
  }
};