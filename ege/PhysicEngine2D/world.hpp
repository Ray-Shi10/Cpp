#ifndef PHYSIC_WORLD_HPP
#define PHYSIC_WORLD_HPP

#include "obj.hpp"
#include <vector>

std::vector<Collision::Info> collInfos;

class World {
  static constexpr float ESP = 1e-6; // small epsilon value
 public:
  vec gravity = vec(0, -120);
  vec size;
  float damping = 0.98; // 阻尼 [0,1]
  std::vector<Object*> objs;
  ~World() {
    for(auto obj : objs) delete obj;
  }
  void add(Object* obj) {
    objs.push_back(obj);
  }
  template <typename ...Args>
  void add(Args ...args) {
    objs.push_back(new Object(args...));
  }
  void step(float dt, int it=1) {
    // const float damping = std::pow(this->damping, dt/it);
    collInfos.clear();
    for(int i=0; i<it; i++) _step(dt/it);
    // concats.clear(); normals.clear();
    _step(dt/it, true);
    // for(auto obj : objs) {
    //   if(!obj->isStatic()) {
    //     applyBoundaries(obj);
    //   }
    // }
  }
  void _step(float dt, bool record=false) {
    for(auto obj : objs) {
      if(!obj->isStatic()) {
        obj->speed.pos += gravity * dt;
        // obj->speed *= damping;
      }
      // obj->trans += obj->speed * dt;
      obj->trans.pos += obj->speed.pos * dt;
      obj->trans.angle += obj->speed.angle * dt;
      // obj->trans.angle *= std::pow(damping, dt);
    }
    
    const int n = objs.size();
    for(int it1=0; it1<n; it1++) {
      auto const o1 = objs[it1];
      for(int it2=it1+1; it2<n; it2++) {
        auto const o2 = objs[it2];
        // std::string _obj1 = o1->toStr(), _obj2 = o2->toStr();
        const auto info = o1->intersect(*o2);
        if(!info) continue;
        if(o1->isStatic() && o2->isStatic()) continue;
        // std::cout << *o1 << "\n" << *o2 << "\n" << info << "\n";
        // info.d *= .9;
        o1->trans.pos -= info.n * info.d / (o1->mass + o2->mass) * o1->mass;
        o2->trans.pos += info.n * info.d / (o1->mass + o2->mass) * o2->mass;
        const auto _info = 1?info:o1->intersect(*o2);
        const vec p = _info.p;
        if(record|1) { collInfos.push_back(info); }
        // std::cout << *o1 << "\n" << *o2 << "\n" << _info << "\n";
        // std::cout << p << " " << o1->trans.pos << " " << o2->trans.pos << " " << _info.d << "\n\n";
        // system("cls");
        const vec ra = p - o1->trans.pos;
        const vec rb = p - o2->trans.pos;
        const vec raPrep = ra.prep();
        const vec rbPrep = rb.prep();
        const vec va = raPrep * o1->speed.angle;
        const vec vb = rbPrep * o2->speed.angle;
        // const vec r1 = (p-o1->trans.pos).prep()*o1->speed.angle + o1->speed.pos;
        // const vec r2 = (p-o2->trans.pos).prep()*o2->speed.angle + o2->speed.pos;
        // const float j = -(1.95) * info.n.dot((o2->speed-o1->speed).pos) / (o1->mass + o2->mass);
        // const vec rel = r2 - r1;
        const vec relv = (o2->speed.pos + vb) - (o1->speed.pos + va);
        const float mag = relv.dot(info.n);
        // if(mag > -ESP) continue; // 防止粘连
        // if(rel.dot(info.n) > 0) continue;
        const float raPDotN = raPrep.dot(info.n);
        const float rbPDotN = rbPrep.dot(info.n);
        float denom = o1->mass + o2->mass;
        denom += raPDotN * raPDotN * o1->inertia + rbPDotN * rbPDotN * o2->inertia;
        const float j = -(1.98) * mag / denom;
        const vec impulse = info.n * j;
        o1->speed.pos -= impulse * o1->mass;
        o2->speed.pos += impulse * o2->mass;
        o1->speed.angle -= Angle(ra.cross(impulse) * o1->inertia);
        o2->speed.angle += Angle(rb.cross(impulse) * o2->inertia);
        // const float j = -(1.95) * info.n.dot(rel) / (o1->mass + o2->mass + 
        //   (info.n.dot(r1))*o1->mass + (info.n.dot(r2))*o2->mass);
        // o1->speed.pos -= info.n * j * o1->mass;
        // o2->speed.pos += info.n * j * o2->mass;
        // o1->speed.angle -= Angle(j * info.n.cross(p-o1->trans.pos) * o1->mass);
        // o2->speed.angle += Angle(j * info.n.cross(p-o2->trans.pos) * o2->mass);
      }
    }
  }
  // void draw(Canvas window) const {
  //   for(auto obj : objs) obj->draw(window);
  // }
  void clear() {
    for(auto obj : objs) delete obj;
    objs.clear();
  }
};

#endif // PHYSIC_WORLD_HPP