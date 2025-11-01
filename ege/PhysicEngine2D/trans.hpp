#ifndef TRANSLATE_HPP
#define TRANSLATE_HPP

#include "vec2.hpp"

class Trans {
 public:
  vec pos;
  Angle angle;
  Trans(const vec& offset, const Angle& angle=Angle()) : pos(offset), angle(angle) {}
  Trans(const Angle& angle) : angle(angle) {}
  Trans() {}
  // vec apply(const vec& p) const { return (p + offset).rot(angle); }
  // vec unapply(const vec& p) const { return p.rot(-angle) - offset; }
  Trans operator-() const { return Trans((-pos).rot(-angle), -angle); }
  Trans operator-(const Trans& other) const {
    return *this + (-other);
  }
  Trans& operator-=(const Trans& other) {
    *this = *this - other;
    return *this;
  }
  Trans operator+(const Trans& other) const {
    return Trans(pos+other.pos.rot(angle), angle+other.angle);
  }
  Trans& operator+=(const Trans& other) {
    *this = *this + other;
    return *this;
  }
  // Trans operator*(float scalar) const {
  //   return Trans(pos * scalar, angle * scalar);
  // }
  // Trans& operator*=(float scalar) {
  //   *this = *this * scalar;
  //   return *this;
  // }
  friend vec operator+(const vec& p, const Trans& t) {
    return p.rot(t.angle) + t.pos;
  }
  friend vec &operator+=(vec& p, const Trans& t) {
    p = p + t;
    return p;
  }
  friend vec operator-(const vec& p, const Trans& t) {
    return p + (-t);
  }
  friend vec &operator-=(vec& p, const Trans& t) {
    p = p - t;
    return p;
  }
  friend std::ostream& operator<<(std::ostream& os, const Trans& t) {
    os << t.toStr();
    return os;
  }
  std::string toStr() const {
    return "Trans(pos=" + pos.toStr() + ", angle=" + angle.toStr() + ")";
  }
  // vec exec(const vec& p) const {
  //   return p.rot(angle) + pos;
  // }
};

#endif // TRANSLATE_HPP