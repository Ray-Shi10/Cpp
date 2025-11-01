#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "vec2.hpp"
#include "color.hpp"
#include "obj.hpp"
#include <string>
#include <ege.h>

class Canvas {
 public:
  vec size;
  vec pos;
  float scale;
  ege::PIMAGE img = nullptr;
  const float aa = 2; // anti-aliasing factor
  Canvas(
    float width, float height,
    const vec& pos=vec(0, 0), float scale=1
  ) : size(width/2, height/2), pos(pos), scale(scale) {
    img = ege::newimage(width*aa, height*aa);
    ege::setcolor(Color(255), img);
    ege::setlinewidth(1*aa, img);
    ege::initgraph(width, height);
  }
  template <typename... Args>
  void clear(const Args &...args) {
    ege::setbkcolor(Color(args...), img);
    ege::cleardevice(img);
  }
  template <typename... Args>
  void setcolor(const Args &...color) const {
    ege::setcolor(Color(color...), img);
  }
  template <typename... Args>
  void setfillcolor(const Args &...color) const {
    ege::setfillcolor(Color(color...), img);
  }
  vec project(const vec& p) const { // world to canvas
    return ((p-pos)*scale*vec(1,-1) + size) * aa;
  }
  vec unproject(const vec& p) const { // canvas to world
    return (p/aa - size) / scale * vec(1,-1) + pos;
  }
  vec unproj(const vec& p) const { // screen to world
    return (p - size) * vec(1,-1);
  }
  vec projVec(const vec& p) const { // world to canvas
    return p * scale * vec(1,-1) * aa;
  }
  float projLen(float l) const { // world to canvas
    return l * scale * aa;
  }
  void circle(vec o, vec r, bool fill=true) const {
    o = project(o); r = projVec(r); o -= r; r *= 2;
    if(fill) ege::ege_fillellipse(o.x, o.y, r.x, r.y, img);
    else ege::ege_ellipse(o.x, o.y, r.x, r.y, img);
  }
  void rect(vec o, vec r, bool fill=true) const {
    r = abs(projVec(r)); o = project(o); o -= r; r *= 2;
    if(fill) ege::ege_fillrect(o.x, o.y, r.x, r.y, img);
    else ege::ege_rectangle(o.x, o.y, r.x, r.y, img);
  }
  void line(vec p1, vec p2) const {
    p1 = project(p1); p2 = project(p2);
    ege::line(p1.x, p1.y, p2.x, p2.y, img);
  }
  vec rand(vec padding=0) const { return vec::rand() * (size - padding) * scale + pos; }
  void render(ege::PIMAGE img = NULL) const {
    ege::putimage(img, 0, 0, size.x*2, size.y*2, this->img, 0, 0, aa*size.x*2, aa*size.y*2);
  }
  void draw(Collision *collision, const Trans &trans) {
    setcolor(Color(255));
    switch(collision->shape) {
      case Shape::Circle: {
        Circle &c = collision->as<Circle>();
        circle(trans.pos, c.radius, true);
        const vec proj = project(trans.pos);
        const float radius = projLen(c.radius);
        ege::circle(proj.x, proj.y, radius, img);
        line(trans.pos, trans.pos + vec::angle(trans.angle)*c.radius);
        break;
      }
      case Shape::Rect: {
        Rect &r = collision->as<Rect>();
        // rect(pos, r.size, true);
        vec points[4] = {
          r.size*vec( 1,  1),
          r.size*vec(-1,  1),
          r.size*vec(-1, -1),
          r.size*vec( 1, -1),
        };
        for(int i=0; i<4; i++) {
          points[i] += trans;
        }
        ege::ege_point pts[4];
        for(int i=0; i<4; i++) {
          vec p = project(points[i]);
          pts[i].x = p.x; pts[i].y = p.y;
        }
        ege::ege_fillpoly(4, pts, img);
        for(int i=0; i<4; i++) {
          ege::line(pts[i].x, pts[i].y, pts[(i+1)%4].x, pts[(i+1)%4].y, img);
        }
        break;
      }
    }
  }
  void draw(Object *obj) {
    // for(Collision *c : obj->collisions) {
      draw(obj->collision, obj->trans);
    // }
  }
};

#endif // WINDOW_HPP