#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <cmath>

class vec {
 public:
  float x, y;
  vec() : x(0), y(0) {}
  // template <> vec<float>(float val) : x(val), y(val) {}
  vec(float val) : x(val), y(val) {}
  vec(float x, float y) : x(x), y(y) {}
  // template <typename T> vec(T v) : x(v.x), y(v.y) {}
  vec  operator-() const { return vec(-x, -y); }
  bool operator==(const vec& v) const { return x == v.x && y == v.y; }
  vec  operator+ (const vec& v) const { return vec(x + v.x, y + v.y); }
  vec& operator+=(const vec& v)       { x += v.x; y += v.y; return *this; }
  vec  operator- (const vec& v) const { return vec(x - v.x, y - v.y); }
  vec& operator-=(const vec& v)       { x -= v.x; y -= v.y; return *this; }
  vec  operator* (float scalar) const { return vec(x * scalar, y * scalar); }
  vec& operator*=(float scalar)      { x *= scalar; y *= scalar; return *this; }
  vec  operator* (const vec& v) const { return vec(x * v.x, y * v.y); }
  vec& operator*=(const vec& v)       { x *= v.x; y *= v.y; return *this; }
  vec  operator/ (float scalar) const { return vec(x / scalar, y / scalar); }
  vec& operator/=(float scalar)      { x /= scalar; y /= scalar; return *this; }
  vec  operator/ (const vec& v) const { return vec(x / v.x, y / v.y); }
  vec& operator/=(const vec& v)       { x /= v.x; y /= v.y; return *this; }
  float dot(const vec& v) const { return x * v.x + y * v.y; }
  float length() const { return std::sqrt(norm()); }
  vec normalize() const { float len = length(); return len > 0 ? *this/len : vec(0, 0); }
  float angle() const { return std::atan2(y, x); }
  float angle(const vec& v) const { return std::acos(dot(v) / (length() * v.length())); }
  float cross(const vec& v) const { return x * v.y - y * v.x; }
  float dist(const vec& v) const { return (*this - v).length(); }
  float norm() const { return dot(*this); }
  friend std::ostream& operator<<(std::ostream& os, const vec& v) { os << v.toStr(); return os; }
  static vec rand01() { return vec(::rand01(), ::rand01()); }
  static vec rand() { return vec::rand01() * 2 - 1; }
  static vec rand(float max) { return vec::rand01() * max; }
  static vec rand(float min, float max) { return vec::rand01() * (max - min) + min; }
  std::string toStr() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
  vec sign() const { return vec(x==0?0:(x>0?1:-1), y==0?0:(y>0?1:-1)); }
  vec perpendicular() const { return vec(-y, x); }
};
vec abs(const vec& v) { return vec(std::abs(v.x), std::abs(v.y)); }
vec min(const vec& a, const vec& b) { return vec(std::fmin(a.x, b.x), std::fmin(a.y, b.y)); }
vec max(const vec& a, const vec& b) { return vec(std::fmax(a.x, b.x), std::fmax(a.y, b.y)); }
// vec clamp(const vec& v, const vec& min, const vec& max) { return ::min(::max(v, min), max); }
vec clamp(const vec& v, const vec& minv, const vec& maxv) {
  return vec(
    v.x < minv.x ? minv.x : (v.x > maxv.x ? maxv.x : v.x),
    v.y < minv.y ? minv.y : (v.y > maxv.y ? maxv.y : v.y)
  );
}

#endif // VECTOR_HPP