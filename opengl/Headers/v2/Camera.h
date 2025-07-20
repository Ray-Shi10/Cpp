#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GLM.h"

class Camera {
public:
    glm::vec3 dir, pos, view;
    glm::real sens_turn, sens_move, sens_zoom;
    glm::real znear, zfar;
    Camera(glm::vec3 pos, glm::vec3 dir, glm::vec3 view, glm::real znear, glm::real zfar, glm::real sens_move, glm::real sens_turn, glm::real sens_zoom) : 
        pos(pos), dir(dir), view(view), znear(znear), zfar(zfar), sens_move(sens_move), sens_turn(sens_turn), sens_zoom(sens_zoom) {}
    glm::mat4 getView() const {
        glm::mat4 view(1.0f);
        view *= glm::rotateXl(-dir.x);
        view *= glm::rotateYl(-dir.y);
        view *= glm::rotateZl(-dir.z);
        view  = glm::translate(view, -pos);
        return view;
    }
    glm::mat4 getProj() const {
        return glm::mat4(
            view.z/view.x, 0.0f         ,  0.0f                       ,  0.0f,
            0.0f         , view.z/view.y,  0.0f                       ,  0.0f,
            0.0f         , 0.0f         , -(zfar+znear)/(zfar-znear), -1.0f, 
            0.0f         , 0.0f         , -(2*zfar*znear)/(zfar-znear), 0.0f
        );
    }
    glm::mat4 getMatrix() const { return getProj() * getView(); }
    void move_f(glm::vec3 v) { pos += v; }
    void move_s(glm::vec3 v) { move_f(sens_move * v); }
    void move(glm::vec3 v, glm::real dt=1.0f) { move_s(dt * glm::rotateYl(dir.y) * glm::vec4(v,1.0f)); }
    void rotate_f(glm::vec3 v) { dir += v; }
    void rotate_s(glm::vec3 v) { rotate_f(v * sens_turn); }
    void rotate(glm::vec3 v, glm::real dt=1.0f) { rotate_s(dt * v);
        if(dir.x >=  glm::PI/2) dir.x  =  glm::PI / 2;
        if(dir.x <= -glm::PI/2) dir.x  = -glm::PI / 2;
        if(dir.y >   glm::PI  ) dir.y -=  glm::PI * 2;
        if(dir.y <  -glm::PI  ) dir.y +=  glm::PI * 2;
        if(dir.z >=  glm::PI  ) dir.z -=  glm::PI * 2;
        if(dir.z <= -glm::PI  ) dir.z +=  glm::PI * 2;
    }
    Camera& operator=(Camera const& c) {
        pos = c.pos; dir = c.dir; view = c.view;
        sens_move = c.sens_move; sens_turn = c.sens_turn; sens_zoom = c.sens_zoom;
        znear = c.znear; zfar = c.zfar;
        return *this;
    }
};

#endif // _CAMERA_H_