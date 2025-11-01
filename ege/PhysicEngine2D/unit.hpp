#ifndef UNIT_HPP
#define UNIT_HPP

#include "vec2.hpp"

enum class UnitType : unsigned int {
  Vect,
  Norm,
  Scalar,
};
template <UnitType type, int time, int dist, int mass>
class Unit {
  bool ASSERT(Unit other) const {
    return type == other.type && time == other.time && dist == other.dist && mass == other.mass;
  }
 public:
  static constexpr int getTime() { return time; }
  static constexpr int getDist() { return dist; }
  static constexpr int getMass() { return mass; }
  float value;
  explicit Unit(float v=0) : value(v) {}
  operator float() const { return value; }
  Unit operator+(const Unit& other) const {
    ASSERT(other);
    return Unit(value + other.value);
  }
  Unit operator-(const Unit& other) const {
    ASSERT(other);
    return Unit(value - other.value);
  }
  Unit operator*(const Unit& other) const {
    return Unit<type,
      time + other.getTime(),
      dist + other.getDist(),
      mass + other.getMass()>(value * other.value);
  }
  Unit operator/(const Unit& other) const {
    return Unit<type,
      time - other.getTime(),
      dist - other.getDist(),
      mass - other.getMass()>(value / other.value);
  }
  Unit& operator+=(const Unit& other) {
    ASSERT(other);
    value += other.value;
    return *this;
  }
  Unit& operator-=(const Unit& other) {
    ASSERT(other);
    value -= other.value;
    return *this;
  }
  Unit& operator*=(const Unit& other) {
    value *= other.value;
    return *this;
  }
  Unit& operator/=(const Unit& other) {
    value /= other.value;
    return *this;
  }
  Unit operator-() const {
    return Unit(-value);
  }
  Unit operator~() const {
    return Unit<type, -time, -dist, -mass>(1.0f / value);
  }
};

#endif // UNIT_HPP