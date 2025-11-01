#include <cmath>
#include <ege.h>
using real = float;
class vec {
public:
  real x, y;
  vec() : x(0), y(0) {}
  vec(real val) : x(val), y(val) {}
  vec(real x, real y) : x(x), y(y) {}
  vec operator-() const { return vec(-x, -y); }
  vec  operator+ (const vec& v) const { return vec(x + v.x, y + v.y); }
  vec& operator+=(const vec& v)       { x += v.x; y += v.y; return *this; }
  vec  operator- (const vec& v) const { return vec(x - v.x, y - v.y); }
  vec& operator-=(const vec& v)       { x -= v.x; y -= v.y; return *this; }
  vec  operator* (real scalar) const { return vec(x * scalar, y * scalar); }
  vec& operator*=(real scalar)      { x *= scalar; y *= scalar; return *this; }
  vec  operator* (const vec& v) const { return vec(x * v.x, y * v.y); }
  vec& operator*=(const vec& v)       { x *= v.x; y *= v.y; return *this; }
  vec  operator/ (real scalar) const { return vec(x / scalar, y / scalar); }
  vec& operator/=(real scalar)      { x /= scalar; y /= scalar; return *this; }
  vec  operator/ (const vec& v) const { return vec(x / v.x, y / v.y); }
  vec& operator/=(const vec& v)       { x /= v.x; y /= v.y; return *this; }
  real dot(const vec& v) const { return x * v.x + y * v.y; }
  real cross(const vec& v) const { return x * v.y - y * v.x; }
  vec normal() const { real l = len(); return l > 0 ? (*this)/l : vec(0, 0); }
  real len() const { return std::sqrt(norm()); }
  real norm() const { return dot(*this); }
};
const vec g(0, -8);
const real ESP = 1e-6;
struct State {
  vec p;
  float lambda;
};
float action(const State &s, const State &v) {
  return v.p.dot(v.p) * .5 - g.dot(s.p) + s.lambda * (s.p.norm() - 1);
}
int main() {
  const vec scr(400, 300);
  ege::initgraph(scr.x*2, scr.y*2);
  const real dt = 1./60;
  const real SCALE = 250;
  for(;ege::is_run();ege::delay_fps(60)) {
    if(ege::kbmsg()) break;
    // s += s.deriv() * dt;
    ege::cleardevice();
    ege::setfillcolor(ege::WHITE);
    ege::fillellipse(
      scr.x + s.p.x * SCALE, scr.y - s.p.y * SCALE,
      10, 10
    );
  }
  return 0;
}