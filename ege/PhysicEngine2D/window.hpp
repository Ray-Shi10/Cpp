#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "vec2.hpp"
#include "color.hpp"
#include <string>
#include <ege.h>

class Screen {
  using str = std::string;
 public:
  vec size;
  vec pos;
  float scale;
  str title;
  ege::PIMAGE img = nullptr;
  const float aa = 2; // anti-aliasing factor
  Screen(
    float width, float height,
    const vec& pos=vec(0, 0), float scale=1,
    const str& title="Physics Engine 2D"
  ) : size(width/2, height/2), pos(pos), scale(scale), title(title) {
    img = ege::newimage(width*aa, height*aa);
    ege::setcolor(Color(255), img);
    ege::setlinewidth(1*aa, img);
    ege::initgraph(width, height);
  }
  void clear(const Color& color=Color(0)) {
    ege::setbkcolor(color, img);
    ege::cleardevice(img);
  }
  vec project(const vec& p) const {
    return ((p-pos)*scale*vec(1,-1) + size) * aa;
  }
  vec unproject(const vec& p) const { //canvas to world
    return (p/aa - size) / scale * vec(1,-1) + pos;
  }
  vec unproj(const vec& p) const { //screen to world
    return (p - size) * vec(1,-1);
  }
  vec projVec(const vec& p) const {
    return p * scale * aa * vec(1,-1);
  }
  void setcaption(const str& title) {
    this->title = title;
    ege::setcaption(title.c_str());
  }
  void circle(vec o, vec r, bool fill) const {
    o = project(o); r = projVec(r); o -= r; r *= 2;
    if(fill) ege::ege_fillellipse(o.x, o.y, r.x, r.y, img);
    else ege::ege_ellipse(o.x, o.y, r.x, r.y, img);
  }
  void rectangle(vec o, vec r, bool fill) const {
    r = abs(projVec(r)); o = project(o); o -= r; r *= 2;
    if(fill) ege::ege_fillrect(o.x, o.y, r.x, r.y, img);
    else ege::ege_rectangle(o.x, o.y, r.x, r.y, img);
  }
  void line(vec p1, vec p2) const {
    p1 = project(p1); p2 = project(p2);
    ege::line(p1.x, p1.y, p2.x, p2.y, img);
  }
  vec rand(vec padding=0) const { return vec::rand() * (size - padding) * scale + pos; }
  void render() const {
    ege::putimage(0, 0, size.x*2, size.y*2, img, 0, 0, aa*size.x*2, aa*size.y*2);
  }
};

#endif // WINDOW_HPP