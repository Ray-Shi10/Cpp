#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vec2.hpp"
// #include "window.hpp"
#include "collision.hpp"
#include "shape.hpp"
#include <ege.h>
#include <iostream>

class Object {
 public:
  Trans trans, speed;
  float mass = 0; // inversed, 0 for static object
  float inertia = 0; // rotational inertia
  // std::vector<Collision*> collisions;
  Collision *collision;
  float restitution = 0.99; // 碰撞恢复系数
  template <typename ...Args>
  Object(const Trans& trans, float mass, Shape shape, Args ...args)
    : trans(trans), collision{Collision::create(shape, args...)} { setMass(mass); }
  // template <typename ...Collisions>
  // Object(const Trans& trans, float mass, Collision *collision, Collisions ...collisions)
  //   : trans(trans), collisions{collision, collisions...} { setMass(mass); }
  template <typename ...Args>
  Object(const Trans& trans, Shape shape, Args ...args)
    : trans(trans), collision(Collision::create(shape, args...)) { setMass(mass); }
  // template <typename ...Collisions>
  // Object(const Trans& trans, Collision *collision, Collisions ...collisions)
  //   : trans(trans), collisions{collision, collisions...} { setMass(mass); }
  Object(const Trans& trans, float mass, Collision *collision)
    : trans(trans), collision(collision) { setMass(mass); }
  Object(const Trans& trans, Collision *collision)
    : trans(trans), collision(collision) { setMass(mass); }

  bool isStatic() const { return mass == 0; }
  Collision::Info intersect(const Object& other) const {
    Collision::Info result;
    const Collision *c1 = collision;
    const Collision *c2 = other.collision;
    const Trans rel = other.trans - trans;
    // for(const Collision *c1 : collisions) {
    //   for(const Collision *c2 : other.collisions) {
        const Collision::Info info = c1->intersect(*c2, rel);
        if(!result || info>result) result = info;
    //   }
    // }
    result.p += trans;
    result.n = result.n.rot(trans.angle);
    return result;
  }
  bool include(const vec& point) const {
    // for(const Collision *c : collisions) {
      if(collision->include(point+trans)) return true;
    // }
    return false;
  }
  float setMass(float m) { return mass = m <= 0 ? 0 : 1/m; }
  float getMass() const { return isStatic() ? 0 : 1/mass; }
  float &getMass() { return mass; }
  friend std::ostream& operator<<(std::ostream& os, const Object& o) {
    os << o.toStr();
    return os;
  }
  std::string toStr() const {
    std::string res =  "Object(trans=" + trans.toStr()
       + ", mass=" + std::to_string(getMass())
       + ", inertia=" + std::to_string(inertia)
       + ", speed=" + speed.toStr();
    // for(const Collision *c : collisions) {
      res += ", " + collision->toStr();
    // }
    return res + ")";
  }

  int name=0;
  std::vector<vec> hist_pos, hist_speed;
};

#endif // OBJECT_HPP