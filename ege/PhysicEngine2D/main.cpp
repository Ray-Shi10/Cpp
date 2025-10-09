#include <iostream>
#include <cmath>
#include <ege.h>
bool pause=1;
#include "phyEngine2D.h"
// using namespace std;

void initWorld(World& world, std::vector<Color>& colors, Canvas& screen) {
  {
    const float R = 8e4, pad = 10;
    world.add(vec(0,  screen.size.y-pad), 0.f, Shape::Rect, vec(screen.size.x, pad)); // U
    world.add(vec(0, -screen.size.y+pad), 0.f, Shape::Rect, vec(screen.size.x, pad)); // D
    world.add(vec( screen.size.x-pad, 0), 0.f, Shape::Rect, vec(pad, screen.size.y)); // L
    world.add(vec(-screen.size.x+pad, 0), 0.f, Shape::Rect, vec(pad, screen.size.y)); // R
    // world.add(new Object(vec(0,  screen.size.y-pad), 0.f, new Rect(vec(screen.size.x, pad)))); // U
    // world.add(new Object(vec(0, -screen.size.y+pad), 0.f, new Rect(vec(screen.size.x, pad)))); // D
    // world.add(new Object(vec( screen.size.x-pad, 0), 0.f, new Rect(vec(pad, screen.size.y)))); // L
    // world.add(new Object(vec(-screen.size.x+pad, 0), 0.f, new Rect(vec(pad, screen.size.y)))); // R
    // world.add(vec(0, -screen.size.y+pad-R), 0.f, Shape::Circle, R); // D
    // world.add(vec(0,  screen.size.y-pad+R), 0.f, Shape::Circle, R); // U
    // world.add(vec( screen.size.x-pad+R, 0), 0.f, Shape::Circle, R); // L
    // world.add(vec(-screen.size.x+pad-R, 0), 0.f, Shape::Circle, R); // R
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
    auto shape = Shape(rand()%2);
    auto *obj = new Object(pos, shape, size);
    // bool ok = 1;
    // for(const auto o2 : world.objs) {
    //   auto info = obj->intersect(*o2);
    //   if(info) {
    //     i--; ok = 0; break;
    //   }
    // }
    // if(!ok) continue;
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
  Canvas screen(800, 600);
  ege::setcaption("Physics Engine 2D");
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
          if(world.objs[i]->include(pos)) {
            id = i;
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
      const float R = 100;
      for(const auto obj : world.objs) {
        if(obj->isStatic()) continue;
        if(obj->trans.pos.dist(mousepos) < R) {
          obj->speed += (obj->trans.pos - mousepos).normalize() * 250;
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
            obj->speed = obj->speed.pos.normalize() * (
              (obj->speed.pos.length() + 15) * 15
            ) + vec::rand() * 35;
          }
        }
        case ege::key_Q: {
          const vec pos = traceId == -1 ? mousepos : world.objs[traceId]->trans.pos;
          const float R = 120;
          for(const auto obj : world.objs) {
            if(obj->isStatic()) continue;
            if(obj->trans.pos.dist(pos) < R) {
              obj->speed.pos += (obj->trans.pos - pos).normalize() * 400;
            }
          }
        }
      }
    }
    if(traceId != -1) { 
      screen.pos = world.objs[traceId]->trans.pos;
      // screen.pos += (world.objs[traceId]->pos - screen.pos) * 5e-2;
    }
    if(!pause) world.step(1.0f/60.0f/(slow?3.5:1), 4);
    screen.clear(Color(20));
    // world.draw();
    for(int i=0; i<world.objs.size(); i++) {
      ege::setfillcolor(colors[i], screen.img);
      // world.objs[i]->draw(screen);
      screen.draw(world.objs[i]);
      const auto o = world.objs[i];
      screen.line(o->trans.pos, o->trans.pos + o->speed.pos*.6);
    }
    for(int i=0; i<world.objs.size(); i++) {
      if(i+1 != world.objs[i]->name) continue;
      char buf[10];
      sprintf(buf, "%d", world.objs[i]->name);
      ege::outtextxy(screen.project(world.objs[i]->trans.pos).x-5, screen.project(world.objs[i]->trans.pos).y-5, buf, screen.img);
    }
    // for(const auto obj : world.objs) {
    //   if(!obj->name) continue;
    //   obj->hist_pos.push_back(obj->trans.pos);
    //   obj->hist_speed.push_back(obj->speed.pos);
    // }
    screen.render();
    ege::setcolor(ege::GRAY);
    ege::line(0, MousePos.y, screen.size.x*2, MousePos.y);
    ege::line(MousePos.x, 0, MousePos.x, screen.size.y*2);
    ege::outtextxy(MousePos.x+1, MousePos.y-16, (std::to_string(int(MousePos.x))+std::string(",")+std::to_string(int(MousePos.y))).c_str());
  }
  ege::closegraph();
  return 0;
}