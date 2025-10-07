#ifndef PHYSIC_WORLD_HPP
#define PHYSIC_WORLD_HPP

#include "obj.hpp"
#include <vector>

class World {
  static constexpr float ESP = 1e-6; // small epsilon value
 public:
  vec gravity = vec(0, -120);
  vec size;
  float damping = 0.7; // 阻尼 [0,1]
  float restitution = 0.3; // 反弹系数 [0,1]
  float minSpeed = 1.0; // 最小速度阈值，防止抖动
  float sleepThreshold = 5.0; // 休眠速度阈值，更高的值
  int sleepFrames = 60; // 需要保持低速的帧数才进入休眠
  std::vector<Object*> objs;
  ~World() {
    for(auto obj : objs) delete obj;
  }
  void add(Object* obj) {
    objs.push_back(obj);
  }
  void step(float dt, int it=1) {
    const float damping = std::pow(this->damping, dt/it);
    for(int i=0; i<it; i++) _step(dt/it, damping);
    
    // Apply world boundaries
    // for(auto obj : objs) {
    //   if(!obj->isStatic()) {
    //     applyBoundaries(obj);
    //   }
    // }
  }
  void _step(float dt, float damping) {
    for(auto obj : objs) {
      if(!obj->isStatic()) {
        obj->speed += gravity * dt;
        // obj->speed *= damping;
      }
      obj->pos += obj->speed * dt;
    }
    
    const int n = objs.size();
    for(int it1=0; it1<n; it1++) {
      auto const o1 = objs[it1];
      for(int it2=it1+1; it2<n; it2++) {
        auto const o2 = objs[it2];
        std::string _obj1 = o1->toStr(), _obj2 = o2->toStr();
        CollInfo info = o1->intersect(*o2);
        if(!info) continue;
        if(o1->isStatic() && o2->isStatic()) continue;
        if((o2->speed-o1->speed).dot(info.n) > 0) continue;
        // info.d *= 1.1;
        o1->pos -= info.n * info.d / (o1->mass + o2->mass) * o1->mass;
        o2->pos += info.n * info.d / (o1->mass + o2->mass) * o2->mass;
        // o1->pos -= info.n * info.d / 2;
        // o2->pos += info.n * info.d / 2;
        const float j = -(1.8) * info.n.dot(o2->speed - o1->speed) / (o1->mass + o2->mass);
        o1->speed -= info.n * j * o1->mass;
        o2->speed += info.n * j * o2->mass;
        info = o1->intersect(*o2);
        // if(info.d > 1e-2) {
        //   // std::cerr << "Warning: Post-collision intersection detected! d=" << info.d << std::endl;
        //   for(int i=0; i<o1->hist_pos.size(); i++) {
        //     std::cout << i << ": " << o1->hist_pos[i] << ", " << o1->hist_speed[i] << "\n";
        //     std::cout << i << ": " << o2->hist_pos[i] << ", " << o2->hist_speed[i] << "\n";
        //   }
        //   std::cout << info << "  " << o1->name << "," << o2->name << "\n" << _obj1 << "\n" << _obj2 << "\n\n";
        //   ::pause = 1;
        // }
      }
    }
  }
  void draw(Screen window) const {
    for(auto obj : objs) obj->draw(window);
  }
  void clear() {
    for(auto obj : objs) delete obj;
    objs.clear();
  }
  void applyBoundaries(Object* obj);
};

#endif // PHYSIC_WORLD_HPP