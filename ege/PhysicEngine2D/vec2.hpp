#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <cmath>

class Angle {
 public:
  float rad;
  constexpr Angle() : rad(0) {}
  constexpr Angle(float rad) : rad(rad) {}
  static constexpr Angle degrees(float deg) { return Angle(deg * PI / 180.0f); }
  constexpr float degrees() const { return rad * 180.0f / PI; }
  constexpr operator float() const { return rad; }
  constexpr bool operator==(const Angle& a) const { return rad == a.rad; }
  constexpr bool operator!=(const Angle& a) const { return rad != a.rad; }
  constexpr Angle operator-() const { return Angle(-rad); }
  constexpr Angle operator+(const Angle& a) const { return Angle(rad + a.rad); }
  constexpr Angle operator-(const Angle& a) const { return Angle(rad - a.rad); }
  constexpr Angle operator*(float scalar) const { return Angle(rad * scalar); }
  constexpr Angle operator/(float scalar) const { return Angle(rad / scalar); }
  constexpr Angle& operator+=(const Angle& a) { rad += a.rad; return *this; }
  constexpr Angle& operator-=(const Angle& a) { rad -= a.rad; return *this; }
  constexpr Angle& operator*=(float scalar) { rad *= scalar; return *this; }
  constexpr Angle& operator/=(float scalar) { rad /= scalar; return *this; }
  friend std::ostream& operator<<(std::ostream& os, const Angle& a) {
    os << a.toStr();
    return os;
  }
  std::string toStr() const {
    return "rad(" + std::to_string(rad) + ")";
  }
};
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
  bool operator<(const vec& v) const { return x < v.x && y < v.y; }
  bool operator<=(const vec& v) const { return x <= v.x && y <= v.y; }
  bool operator>(const vec& v) const { return x > v.x && y > v.y; }
  bool operator>=(const vec& v) const { return x >= v.x && y >= v.y; }
  float dot(const vec& v) const { return x * v.x + y * v.y; }
  float length() const { return std::sqrt(norm()); }
  vec normalize() const { float len = length(); return len > 0 ? *this/len : vec(0, 0); }
  float angle() const { return std::atan2(y, x); }
  float angle(const vec& v) const { return std::acos(dot(v) / (length() * v.length())); }
  vec rot(const Angle& a) const {
    const float c = std::cos(a.rad), s = std::sin(a.rad);
    return vec(x * c - y * s, x * s + y * c);
  }
  vec rot(const vec& center, const Angle& a) const {
    return (*this - center).rot(a) + center;
  }
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