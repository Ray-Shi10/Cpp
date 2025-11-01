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
    Info() : n(0,0), d(-1), p(0,0) {}
    Info(const vec& n, float d, const vec& p)
      : n(n), d(d), p(p) {}
    operator bool() const { return d > 0; }
    operator float() const { return d; }
    bool operator>(const Info& other) const { return d > other.d; }
    bool operator<(const Info& other) const { return d < other.d; }
    friend std::ostream& operator<<(std::ostream& os, const Info& info) {
      os << "Info(n=" << info.n << ", d=" << info.d << ")";
      return os;
    }
    // Info operator-() const { return Info(-n, d, p); }
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
    Info res;
    if(int(other.shape) > int(shape)) {
      res = other._intersect(*this, -trans);
      const Trans t = trans + Trans(res.p, res.n.angle());
      res.p = t.pos;
      res.n = -vec::angle(t.angle);
    } else {
      res = this->_intersect(other, trans);
    }
    return res;
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
      default: error("Circle-Rect intersection not implemented"); break;
    }
    return Info();
  }
  std::string _toStr() const override {
    return "size=" + std::to_string(radius);
  }
};
class Range {
 public:
  float min, max;
  Range(float min, float max) : min(min), max(max) {}
  Range operator&(const Range& other) const {
    return Range(std::fmax(min, other.min), std::fmin(max, other.max));
  }
  operator float() const { return max - min; }
};
Range SAT(vec axis, const std::vector<vec>& points) {
  float min = 1e9;
  float max = -1e9;
  for(const vec& p : points) {
    float proj = p.dot(axis);
    min = std::fmin(min, proj);
    max = std::fmax(max, proj);
  }
  return Range(min, max);
}
Range SAT(vec axis, vec p, float r) {
  float proj = p.dot(axis);
  return Range(proj - r, proj + r);
}
int closestPoint(const vec& point, const std::vector<vec>& points) {
  int closestID = -1;
  float closestDist = 1e9;
  for(int i=0; i<points.size(); i++) {
    const float dist = point.dist(points[i]);
    if(dist < closestDist) {
      closestDist = dist;
      closestID = i;
    }
  }
  return closestID;
}
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
        const std::vector<vec> points = {
          size*vec( 1,  1),
          size*vec(-1,  1),
          size*vec(-1, -1),
          size*vec( 1, -1),
        };
        std::vector<vec> axiss = {
          vec(1,0), vec(0,1), (pos-closestPoint(pos, points)).normalize()
        };
        float minDepth = 1e9;
        vec minAxis;
        for(const vec& axis : axiss) {
          const Range r1 = SAT(axis, points);
          const Range r2 = SAT(axis, pos, c.radius);
          const Range overlap = r1 & r2;
          if(overlap.min >= overlap.max) return Info();
          const float depth = overlap;
          if(depth < minDepth) {
            minDepth = depth;
            minAxis = axis;
          }
        }
        if(pos.dot(minAxis) < 0) minAxis = -minAxis;
        // minAxis = pos.sign() * abs(minAxis).normalize();
        return Info(minAxis, minDepth, pos - minAxis * c.radius);
      }
      case Shape::Rect: {
        const Rect& r = other.as<Rect>();
        const std::vector<vec> points1 = {
          size*vec( 1,  1),
          size*vec(-1,  1),
          size*vec(-1, -1),
          size*vec( 1, -1),
        };
        const std::vector<vec> points2 = {
          r.size*vec( 1,  1) + trans,
          r.size*vec(-1,  1) + trans,
          r.size*vec(-1, -1) + trans,
          r.size*vec( 1, -1) + trans,
        };
        std::vector<vec> axiss = {
          vec(1,0), vec(0,1), vec::angle(rot), vec::angle(rot).prep()
        };
        float minDepth = 1e9;
        vec minAxis;
        for(const vec& axis : axiss) {
          const Range r1 = SAT(axis, points1);
          const Range r2 = SAT(axis, points2);
          const Range overlap = r1 & r2;
          if(overlap.min >= overlap.max) return Info();
          const float depth = overlap;
          if(depth < minDepth) {
            minDepth = depth;
            minAxis = axis;
          }
        }
        if(pos.dot(minAxis) < 0) minAxis = -minAxis;
        const int id = closestPoint(vec(0,0), points2);
        return Info(minAxis, minDepth, points2[id]);
        // vec dir = pos.sign();
        // vec diff = abs(pos);
        // vec total = size + r.size;
        // vec overlap = total - diff;
        // if(overlap.x > 0 && overlap.y > 0) {
        //   if(overlap.x < overlap.y) {
        //     vec n = diff.x < 0 ? vec(-1, 0) : vec(1, 0);
        //     return Info(n*dir, overlap.x, (size-overlap/2) * dir);
        //   } else {
        //     vec n = diff.y < 0 ? vec(0, -1) : vec(0, 1);
        //     return Info(n*dir, overlap.y, (size-overlap/2) * dir);
        //   }
        // }
        return Info();
      }
      default: error("Rect-Unknown intersection not implemented"); break;
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