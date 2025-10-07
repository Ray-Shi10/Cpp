#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vec2.hpp"
#include "window.hpp"
#include <ege.h>
#include <iostream>

class CollInfo {
 public:
  vec n;
  float d;
  vec p;
  CollInfo(const vec& n=vec(), float d=0, const vec& p=vec())
    : n(n), d(d), p(p) {}
  operator bool() const { return d > 0; }
  friend std::ostream& operator<<(std::ostream& os, const CollInfo& info) {
    os << "CollInfo(n=" << info.n << ", d=" << info.d << ")";
    return os;
  }
  CollInfo operator-() const { return CollInfo(-n, d, p); }
};
class Shape {
  using type = unsigned int;
  type val;
 public:
  enum : type {
    Circle, Rect
  ,___END};
  constexpr static int MIN_VALUE = 0, MAX_VALUE = ___END - 1;
  constexpr Shape(type val) : val(val) {
    if(val > MAX_VALUE) throw std::invalid_argument("Invalid Shape value");
  }
  constexpr bool is(type v) const { return val == v; }
  constexpr bool operator==(const Shape& other) const { return val == other.val; }
  constexpr bool operator!=(const Shape& other) const { return val != other.val; }
  constexpr bool operator>(const Shape& other) const { return val > other.val; }
  constexpr bool operator<(const Shape& other) const { return val < other.val; }
  constexpr bool operator>=(const Shape& other) const { return val >= other.val; }
  constexpr bool operator<=(const Shape& other) const { return val <= other.val; }
  constexpr operator type() const { return val; }
  Shape operator+() = delete;
  Shape operator-() = delete;
  Shape operator+(const Shape&) = delete;
  Shape operator-(const Shape&) = delete;
  Shape operator*(const Shape&) = delete;
  Shape operator/(const Shape&) = delete;
  Shape operator%(const Shape&) = delete;
#define CASE(a) case a: return #a;
  std::string toStr() const {
    switch(val) {
      CASE(Circle)
      CASE(Rect)
      default: return "UNKNOWN";
    }
  }
#undef CASE
};
class Object {
 public:
  vec pos;
  vec speed;
  float mass; // inversed, 0 for static object
  const Shape shape;
  float restitution = 0.99; // 碰撞恢复系数
  
  bool isStatic() const { return mass == 0; }
  virtual CollInfo _intersect(const Object&) const = 0;
  CollInfo intersect(const Object& other) const {
    if(other.shape > this->shape) {
      return -other._intersect(*this);
    }
    return this->_intersect(other);
  }
  template <typename T> T &as() { return dynamic_cast<T&>(*this); }
  template <typename T> const T& as() const { return dynamic_cast<const T&>(*this); }
  virtual void draw(Screen) const = 0;
  float setMass(float m) { return mass = m <= 0 ? 0 : 1/m; }
  float getMass() const { return isStatic() ? 0 : 1/mass; }
  float &getMass() { return mass; }
  friend std::ostream& operator<<(std::ostream& os, const Object& o) {
    os << o.toStr();
    return os;
  }
  std::string toStr() const {
    return shape.toStr()
       + "(pos=" + pos.toStr() + ", speed=" + speed.toStr()
       + ", mass=" + std::to_string(getMass()) + _otherLog() + ")";
  }

  int name=0;
  std::vector<vec> hist_pos, hist_speed;
 protected:
  Object(Shape shape, const vec& pos, float m, const vec& speed=vec())
    : shape(shape), pos(pos), speed(speed), mass(m<=0?-m:1/m) {}
  virtual std::string _otherLog() const { return ""; }
};
class Circle : public Object {
 public:
  float radius;
  Circle() : Circle(vec(), 0, 0) {}
  Circle(const vec& pos, float r, float m) : 
    radius(r), Object(Shape::Circle, pos, m) {}
  Circle(float x, float y, float r, float m) :
    Circle(vec(x, y), r, m) {}
  Circle(float x, float y, float r) :
    Circle(vec(x, y), r, r*r*ege::PI) {}
  Circle(const vec& pos, float r) :
    Circle(pos, r, r*r*ege::PI) {}
  CollInfo _intersect(const Object& other) const override {
    switch(other.shape) {
      case Shape::Circle: {
        const Circle& c = other.as<Circle>();
        const float dist = pos.dist(c.pos);
        if(dist < radius + c.radius) {
          const vec n = (c.pos - pos).normalize();
          const float d = radius + c.radius - dist;
          return CollInfo(n, d, pos + n * radius);
        } else return CollInfo();
      }
    }
    return CollInfo();
  }
  void draw(Screen window) const override {
    window.circle(pos, radius, true);
  }
 protected:
  std::string _otherLog() const override {
    return ", radius=" + std::to_string(radius);
  }
};
class Rect : public Object {
 public:
  vec size;
  Rect() : Rect(vec(), vec(), 0) {}
  Rect(const vec& pos, const vec& size, float m=0) :
    size(size), Object(Shape::Rect, pos, m) {
  }
  CollInfo _intersect(const Object& other) const override {
    switch(other.shape) {
      case Shape::Circle: {
        const Circle& c = other.as<Circle>();
        vec points[] = {
          pos + size*vec( 1,  1),
          pos + size*vec(-1,  1),
          pos + size*vec(-1, -1),
          pos + size*vec( 1, -1),
        };
        vec minAxis;
        float minDepth = 1e9;
        for(int i=0; i<4; i++) {
          const vec p1 = points[i];
          const vec p2 = points[(i+1)%4];
          const vec edge = p2 - p1;
          const vec axis = edge.perpendicular().normalize();
          float minR = 1e9, maxR = -1e9;
          float minC = 1e9, maxC = -1e9;
          for(int j=0; j<4; j++) {
            const float proj = points[j].dot(axis);
            minR = std::fmin(minR, proj);
            maxR = std::fmax(maxR, proj);
          }
          const float proj = c.pos.dot(axis);
          minC = proj - c.radius;
          maxC = proj + c.radius;
          if(maxR <= minC || maxC <= minR) return CollInfo();
          const float depth = std::fmin(maxR - minC, maxC - minR);
          if(depth < minDepth) {
            minDepth = depth;
            minAxis = axis;
          }
        }
        int closestID = -1;
        float closestDist = 1e9;
        for(int i=0; i<4; i++) {
          const float dist = c.pos.dist(points[i]);
          if(dist < closestDist) {
            closestDist = dist;
            closestID = i;
          }
        }
        vec closest = points[closestID];
        vec axis = (c.pos - closest).normalize();
        // if(axis.dot(minAxis) < 0) axis = -axis;
        float minR = 1e9, maxR = -1e9;
        float minC = 1e9, maxC = -1e9;
        for(int j=0; j<4; j++) {
          const float proj = points[j].dot(axis);
          minR = std::fmin(minR, proj);
          maxR = std::fmax(maxR, proj);
        }
        const float proj = c.pos.dot(axis);
        minC = proj - c.radius;
        maxC = proj + c.radius;
        if(maxR <= minC || maxC <= minR) return CollInfo();
        const float depth = std::fmin(maxR - minC, maxC - minR);
        if(depth < minDepth) {
          minDepth = depth;
          minAxis = axis;
        }
        vec direction = c.pos - pos;
        if(direction.dot(minAxis) < 0) minAxis = -minAxis;
        minAxis = direction.sign() * abs(minAxis).normalize();
        return CollInfo(minAxis, minDepth, c.pos - minAxis * c.radius);
      }
      case Shape::Rect: {
        const Rect& r = other.as<Rect>();
        vec diff = r.pos - pos;
        vec dir = diff.sign();
        diff = abs(diff);
        vec total = size + r.size;
        vec overlap = total - diff;
        if(overlap.x > 0 && overlap.y > 0) {
          if(overlap.x < overlap.y) {
            vec n = diff.x < 0 ? vec(-1, 0) : vec(1, 0);
            return CollInfo(n*dir, overlap.x, pos + n * size.x * dir);
          } else {
            vec n = diff.y < 0 ? vec(0, -1) : vec(0, 1);
            return CollInfo(n*dir, overlap.y, pos + n * size.y * dir);
          }
        }
        return CollInfo();
      }
    }
    return CollInfo();
  }
  void draw(Screen window) const override {
    window.rectangle(pos, size, true);
  }
 protected:
  std::string _otherLog() const override {
    return ", size=" + size.toStr();
  }
};

#endif // OBJECT_HPP