#include <iostream>
#include <cmath>
#include <ege.h>
bool pause=1;
#include "phyEngine2D.h"
// using namespace std;

void initWorld(World& world, std::vector<Color>& colors, Screen& screen) {
  {
    const float R = 8e4, pad = 10;
    // world.add(new Circle(0,  screen.size.y-pad+R, R, 0)); // U
    // world.add(new Circle(0, -screen.size.y+pad-R, R, 0)); // D
    // world.add(new Circle( screen.size.x-pad+R, 0, R, 0)); // L
    // world.add(new Circle(-screen.size.x+pad-R, 0, R, 0)); // R
    world.add(new Rect(vec(0,  screen.size.y-pad), vec(screen.size.x, pad), 0)); // U
    world.add(new Rect(vec(0, -screen.size.y+pad), vec(screen.size.x, pad), 0)); // D
    world.add(new Rect(vec( screen.size.x-pad, 0), vec(pad, screen.size.y), 0)); // L
    world.add(new Rect(vec(-screen.size.x+pad, 0), vec(pad, screen.size.y), 0)); // R
    for(int i=0; i<world.objs.size(); i++) colors.push_back(Color(80));
  }
  {
    // const int R = 8;
    // for(int i=-screen.size.x; i<=screen.size.x; i+=R*2) {
    //   world.add(new Circle(i, -screen.size.y+R, R, 0)); // D
    //   colors.push_back(Color(80));
    // }
  }
  for(int i=0; i<30; i++) {
    const float size = rand()%20 + 10;
    const vec pos = screen.rand(80);
    Shape shape = rand()%2;
    Object *obj;
    // shape = Shape::CIRCLE;
    switch(shape) {
      case Shape::Circle:
        obj = new Circle(pos, size);
        break;
      case Shape::Rect:
        obj = new Rect(pos, vec::rand(.8,1.2)*size);
        break;
    }
    bool ok = 1;
    for(const auto o2 : world.objs) {
      CollInfo info = obj->intersect(*o2);
      if(info) {
        i--; ok = 0; break;
      }
    }
    if(!ok) continue;
    if(pos.y < -10 && !(rand()%2)) {
      obj->mass = 0;
      colors.push_back(Color(80));
    } else {
      switch(shape) {
        case Shape::Circle:
          obj->setMass(size*size*3.14);
          break;
        case Shape::Rect:
          obj->setMass(size*size*4);
          break;
      }
      colors.push_back(Color::rand(120, 250));
    }
    obj -> name = i+1;
    world.add(obj);
  }
}

int main() {
  World world;
  Screen screen(800, 600);
  world.restitution = 0.5;
  world.damping = 0.98;
  world.minSpeed = 0;
  srand(time(NULL));
  std::vector<Color> colors;
  initWorld(world, colors, screen);
  bool mousedown[2] = {false};
  vec mousepos, MousePos;
  int traceId = -1;
  bool slow = false;
  for(;ege::is_run();ege::delay_fps(60)) {
    while(ege::mousemsg()) {
      const auto msg = ege::getmouse();
      if(msg.is_down() && msg.is_left()) {
        vec pos = screen.unproj(vec(msg.x, msg.y));
        if(traceId >= 0) { continue; }
        int id = -1;
        for(int i=0; i<world.objs.size()&&id<0; i++) {
          switch(world.objs[i]->shape) {
            case Shape::Circle: {
              const Circle &c = world.objs[i]->as<Circle>();
              if(c.pos.dist(pos) <= c.radius) {
                id = i;
              }
              break;
            }
            case Shape::Rect: {
              const Rect &r = world.objs[i]->as<Rect>();
              const vec diff = pos - r.pos;
              if(std::abs(diff.x) <= r.size.x && std::abs(diff.y) <= r.size.y) {
                id = i;
              }
              break;
            }
          }
        }
        traceId = id;
        mousedown[0] = 1;
        if(id == -1) {
          screen.scale = 5;
          screen.pos = pos;
        } else {
          screen.scale = 1.8;
        }
      }
      if(msg.is_up() && msg.is_left()) {
        mousedown[0] = 0;
        screen.scale = 1;
        screen.pos = vec(0, 0);
        traceId = -1;
      }
      if(msg.is_move() && mousedown[0]) {
        screen.pos = screen.unproj(vec(msg.x, msg.y));
      }
      if(msg.is_move()) {
        mousepos = screen.unproj(vec(msg.x, msg.y));
        MousePos = vec(msg.x, msg.y);
      }
      if(msg.is_up() && msg.is_right()) {
        mousedown[1] = 0;
      }
      if(msg.is_down() && msg.is_right()) {
        mousedown[1] = 1;
      }
    }
    if(mousedown[1]) {
      Circle c = Circle(mousepos, 50, 0);
      for(const auto obj : world.objs) {
        if(obj->isStatic()) continue;
        CollInfo info = c.intersect(*obj);
        if(info) {
          obj->speed += info.n * 400;
        }
      }
    }
    while(ege::kbmsg()) {
      const auto msg = ege::getkey();
      static const float speed = 1.2 * 60;
      if(msg.msg & ege::key_msg_char) continue;
      if(!(msg.flags & ege::key_flag_first_down)) continue;
      switch(msg.key) {
        // case ege::key_W: player.speed.y += msg.msg & ege::key_msg_down ? speed : -speed; break;
        // case ege::key_A: player.speed.x -= msg.msg & ege::key_msg_down ? speed : -speed; break;
        // case ege::key_S: player.speed.y -= msg.msg & ege::key_msg_down ? speed : -speed; break;
        // case ege::key_D: player.speed.x += msg.msg & ege::key_msg_down ? speed : -speed; break;
        case ege::key_esc: ege::closegraph(); return 0;
        case ege::key_enter: slow = !slow; break;
        case ege::key_space: pause = !pause; break;
        case ege::key_tab: world.step(1.0f/60.0f/(slow?3.5:1), 4); break;
        case ege::key_R: world.clear(); colors.clear(); initWorld(world, colors, screen); break;
        case ege::key_X: {
          for(const auto obj : world.objs) {
            if(obj->isStatic()) continue;
            obj->speed = obj->speed.normalize() * (
              (obj->speed.length() + 15) * 15
            ) + vec::rand() * 35;
          }
        }
        case ege::key_Q: {
          const vec pos = traceId == -1 ? mousepos : world.objs[traceId]->pos;
          Circle* c = new Circle(pos, 120, 0);
          for(const auto obj : world.objs) {
            if(obj->isStatic()) continue;
            CollInfo info = c->intersect(*obj);
            if(info) {
              obj->speed += info.n * 1e3 * (2 - info.d/100);
            }
          }
        }
      }
    }
    if(traceId != -1) { 
      screen.pos = world.objs[traceId]->pos;
      // screen.pos += (world.objs[traceId]->pos - screen.pos) * 5e-2;
    }
    if(!pause) world.step(1.0f/60.0f/(slow?3.5:1), 4);
    screen.clear(Color(20));
    // world.draw();
    for(int i=0; i<world.objs.size(); i++) {
      ege::setfillcolor(colors[i], screen.img);
      world.objs[i]->draw(screen);
      const auto o = world.objs[i];
      screen.line(o->pos, o->pos + o->speed*.6);
    }
    for(int i=0; i<world.objs.size(); i++) {
      if(i+1 != world.objs[i]->name) continue;
      char buf[10];
      sprintf(buf, "%d", world.objs[i]->name);
      ege::outtextxy(screen.project(world.objs[i]->pos).x-5, screen.project(world.objs[i]->pos).y-5, buf, screen.img);
    }
    for(const auto obj : world.objs) {
      if(!obj->name) continue;
      obj->hist_pos.push_back(obj->pos);
      obj->hist_speed.push_back(obj->speed);
    }
    screen.render();
    ege::setcolor(ege::WHITE);
    ege::line(0, MousePos.y, screen.size.x*2, MousePos.y);
    ege::line(MousePos.x, 0, MousePos.x, screen.size.y*2);
    ege::outtextxy(MousePos.x+1, MousePos.y-16, (std::to_string(int(MousePos.x))+std::string(",")+std::to_string(int(MousePos.y))).c_str());
  }
  ege::closegraph();
  return 0;
}