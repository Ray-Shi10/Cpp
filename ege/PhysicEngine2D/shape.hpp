#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <string>
#include <stdexcept>

// class Shape {
//  public:
//   using type = unsigned int;
//   type val;
//   // static Shape Circle, Rect;
//   constexpr static Shape Circle = Shape(0);
//   constexpr static Shape Rect   = Shape(1);
//   constexpr static int MIN_VALUE = 0, MAX_VALUE = 1;
//   explicit constexpr Shape(type val) : val(val) {
//     if(val > MAX_VALUE) throw std::invalid_argument("Invalid Shape value");
//   }
//   constexpr bool is(type v) const { return val == v; }
//   constexpr bool operator==(const Shape& other) const { return val == other.val; }
//   constexpr bool operator!=(const Shape& other) const { return val != other.val; }
//   constexpr bool operator>(const Shape& other) const { return val > other.val; }
//   constexpr bool operator<(const Shape& other) const { return val < other.val; }
//   constexpr bool operator>=(const Shape& other) const { return val >= other.val; }
//   constexpr bool operator<=(const Shape& other) const { return val <= other.val; }
//   constexpr operator type() const { return val; }
//   Shape operator+() = delete;
//   Shape operator-() = delete;
//   Shape operator+(const Shape&) = delete;
//   Shape operator-(const Shape&) = delete;
//   Shape operator*(const Shape&) = delete;
//   Shape operator/(const Shape&) = delete;
//   Shape operator%(const Shape&) = delete;
// #define CASE(a) case a: return #a;
//   std::string toStr() const {
//     switch(val) {
//       CASE(Circle)
//       CASE(Rect)
//       default: return "UNKNOWN";
//     }
//   }
// #undef CASE
// };
// Shape Shape::Circle = Shape(0);
// Shape Shape::Rect   = Shape(1);
enum class Shape : unsigned int {
  Circle,
  Rect,
};
std::string toStr(Shape shape) {
  switch(shape) {
    case Shape::Circle: return "Circle";
    case Shape::Rect:   return "Rect";
    default: return "UNKNOWN";
  }
}

#endif // SHAPE_HPP