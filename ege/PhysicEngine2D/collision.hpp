#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "shape.hpp"
#include "trans.hpp"
#include "vec2.hpp"

class Collision {
 public:
  class Info {
   public:
    vec n; float d; vec p;
    Info(const vec& n=vec(), float d=-1, const vec& p=vec())
      : n(n), d(d), p(p) {}
    operator bool() const { return d > 0; }
    operator float() const { return d; }
    bool operator>(const Info& other) const { return d > other.d; }
    bool operator<(const Info& other) const { return d < other.d; }
    friend std::ostream& operator<<(std::ostream& os, const Info& info) {
      os << "Info(n=" << info.n << ", d=" << info.d << ")";
      return os;
    }
    Info operator-() const { return Info(-n, d, p); }
  };
  const Shape shape;
  Collision(Shape shape) : shape(shape) {}
  // Collision(Shape::type shape) : shape(Shape(shape)) {}
  template <typename ...Args>
  static Collision *create(Shape shape, const Args &...args);
  Info intersect(const Collision& other, const Trans &trans) const {
    // if(shape==Shape::Rect && other.shape==Shape::Circle ||
    //    shape==Shape::Circle && other.shape==Shape::Rect) {
    //   std::cout << "Rect-Circle intersect " << int(shape) << " " << int(other.shape) << " " << ::toStr(int(shape)<int(other.shape)) << "\n";
    // }
    if(int(other.shape) > int(shape)) {
      return -other._intersect(*this, -trans);
    }
    return this->_intersect(other, trans);
  }
  virtual bool include(const vec&) const = 0;
  template <typename T> T &as() { return dynamic_cast<T&>(*this); }
  template <typename T> const T& as() const { return dynamic_cast<const T&>(*this); }
  std::string toStr() const {
    return ::toStr(shape) + "(" + _toStr() + ")";
  }
 protected:
  virtual Info _intersect(const Collision&, const Trans&) const = 0;
  virtual std::string _toStr() const { return ""; }
};
class Circle : public Collision {
 public:
  float radius;
  Circle(float r = 0) : radius(r), Collision(Shape::Circle) {}
  bool include(const vec& point) const override {
    return point.norm() <= radius * radius;
  }
  template <typename ...Args> Circle(Args ...args) : Collision(Shape::Circle) { throw std::invalid_argument("Cannot construct Collision with given arguments"); }
 protected:
  Info _intersect(const Collision& other, const Trans& trans) const override {
    const vec pos = trans.pos;
    switch(other.shape) {
      case Shape::Circle: {
        const Circle& c = other.as<Circle>();
        const float dist = vec().dist(pos);
        if(dist < radius + c.radius) {
          const vec n = pos.normalize();
          const float d = radius + c.radius - dist;
          return Info(n, d, pos + n * radius);
        } else return Info();
      }
    }
    return Info();
  }
  std::string _toStr() const override {
    return "size=" + std::to_string(radius);
  }
};
class Rect : public Collision {
 public:
  vec size;
  Rect(const vec& size) : size(size), Collision(Shape::Rect) {}
  Rect(float size) : size(size), Collision(Shape::Rect) {}
  bool include(const vec& point) const override {
    return abs(point) <= size;
  }
  template <typename ...Args> Rect(Args ...args) : Collision(Shape::Rect) { throw std::invalid_argument("Cannot construct Collision with given arguments"); }
 protected:
  Info _intersect(const Collision& other, const Trans& trans) const override {
    const vec pos = trans.pos;
    const Angle rot = trans.angle;
    switch(other.shape) {
      case Shape::Circle: {
        // std::cout << "Rect-Circle intersect called\n";
        const Circle& c = other.as<Circle>();
        vec points[] = {
          size*vec( 1,  1),
          size*vec(-1,  1),
          size*vec(-1, -1),
          size*vec( 1, -1),
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
          const float proj = pos.dot(axis);
          minC = proj - c.radius;
          maxC = proj + c.radius;
          if(maxR <= minC || maxC <= minR) return Info();
          const float depth = std::fmin(maxR - minC, maxC - minR);
          if(depth < minDepth) {
            minDepth = depth;
            minAxis = axis;
          }
        }
        int closestID = -1;
        float closestDist = 1e9;
        for(int i=0; i<4; i++) {
          const float dist = pos.dist(points[i]);
          if(dist < closestDist) {
            closestDist = dist;
            closestID = i;
          }
        }
        vec closest = points[closestID];
        vec axis = (pos - closest).normalize();
        // if(axis.dot(minAxis) < 0) axis = -axis;
        float minR = 1e9, maxR = -1e9;
        float minC = 1e9, maxC = -1e9;
        for(int j=0; j<4; j++) {
          const float proj = points[j].dot(axis);
          minR = std::fmin(minR, proj);
          maxR = std::fmax(maxR, proj);
        }
        const float proj = pos.dot(axis);
        minC = proj - c.radius;
        maxC = proj + c.radius;
        if(maxR <= minC || maxC <= minR) return Info();
        const float depth = std::fmin(maxR - minC, maxC - minR);
        if(depth < minDepth) {
          minDepth = depth;
          minAxis = axis;
        }
        if(pos.dot(minAxis) < 0) minAxis = -minAxis;
        minAxis = pos.sign() * abs(minAxis).normalize();
        return Info(minAxis, minDepth, pos - minAxis * c.radius);
      }
      case Shape::Rect: {
        const Rect& r = other.as<Rect>();
        vec dir = pos.sign();
        vec diff = abs(pos);
        vec total = size + r.size;
        vec overlap = total - diff;
        if(overlap.x > 0 && overlap.y > 0) {
          if(overlap.x < overlap.y) {
            vec n = diff.x < 0 ? vec(-1, 0) : vec(1, 0);
            return Info(n*dir, overlap.x, pos + n * size.x * dir);
          } else {
            vec n = diff.y < 0 ? vec(0, -1) : vec(0, 1);
            return Info(n*dir, overlap.y, pos + n * size.y * dir);
          }
        }
        return Info();
      }
    }
    return Info();
  }
  std::string _toStr() const override {
    return "size=" + size.toStr();
  }
};
template <typename ...Args>
Collision *Collision::create(Shape shape, const Args &...args) {
  switch(shape) {
    case Shape::Rect:   return new Rect(args...);
    case Shape::Circle: return new Circle(args...);
    default: throw std::invalid_argument("Invalid Shape for Collision");
  }
}

#endif // COLLISION_HPP