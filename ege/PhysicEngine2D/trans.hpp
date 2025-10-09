#ifndef TRANSLATE_HPP
#define TRANSLATE_HPP

#include "vec2.hpp"

class Trans {
 public:
  vec pos;
  Angle angle;
  Trans(const vec& offset=vec(), const Angle& angle=Angle()) : pos(offset), angle(angle) {}
  // vec apply(const vec& p) const { return (p + offset).rot(angle); }
  // vec unapply(const vec& p) const { return p.rot(-angle) - offset; }
  Trans operator-() const { return Trans(-pos, -angle); }
  Trans operator-(const Trans& other) const {
    return Trans(pos-other.pos, angle - other.angle);
  }
  Trans& operator-=(const Trans& other) {
    pos -= other.pos;
    angle -= other.angle;
    return *this;
  }
  Trans operator+(const Trans& other) const {
    return Trans(pos + other.pos.rot(angle), angle + other.angle);
  }
  Trans& operator+=(const Trans& other) {
    pos += other.pos.rot(angle);
    angle += other.angle;
    return *this;
  }
  Trans operator*(float scalar) const {
    return Trans(pos * scalar, angle * scalar);
  }
  Trans& operator*=(float scalar) {
    pos *= scalar;
    angle *= scalar;
    return *this;
  }
  friend std::ostream& operator<<(std::ostream& os, const Trans& t) {
    os << t.toStr();
    return os;
  }
  std::string toStr() const {
    return "Trans(offset=" + pos.toStr() + ", angle=" + angle.toStr() + ")";
  }
};

#endif // TRANSLATE_HPP