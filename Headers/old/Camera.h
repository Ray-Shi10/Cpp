#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <GLM.h>
//#include <Input.h>

class Camera {
public:
    glm::vec3 dir, pos, view;
    float sens_turn, sens_move, sens_zoom;
    float znear, zfar;
    Camera(glm::vec3 pos, glm::vec3 dir, glm::vec3 view, glm::real znear, glm::real zfar, glm::real sens_move, glm::real sens_turn, glm::real sens_zoom) : 
        pos(pos), dir(dir), view(view), znear(znear), zfar(zfar), sens_move(sens_move), sens_turn(sens_turn), sens_zoom(sens_zoom) {}
    glm::mat4 getViewMatrix() const {
        glm::mat4 view = glm::mat4(1.0f);/*
        view = glm::rotate(view, -dir.x, glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, -dir.y, glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, -dir.z, glm::vec3(0.0f, 0.0f, 1.0f));/*/
        view *= glm::rotateXl(-dir.x);
        view *= glm::rotateYl(-dir.y);
        view *= glm::rotateZl(-dir.z);//*/
        view = glm::translate(view, -pos);
        return view;
    }
    glm::mat4 getProjectionMatrix() const {
        return glm::mat4(
            view.z/view.x, 0.0f, 0.0f, 0.0f,
            0.0f, view.z/view.y, 0.0f, 0.0f,
            0.0f, 0.0f, -(zfar+znear)/(zfar-znear), -1.0f,
            0.0f, 0.0f, -(2*zfar*znear)/(zfar-znear), 0.0f
        );
    }
    glm::mat4 getMatrix() const {
        return getProjectionMatrix() * getViewMatrix();
    }
    void move_f(glm::vec3 v) {
        //if(glm::length(v)) printf("        Camera pos before: %d %d %d\n", pos.x, pos.y, pos.z);
        pos += v;
        //if(glm::length(v)) printf("        Camera move: %d %d %d\n        Camera pos: %d %d %d", v.x, v.y, v.z, pos.x, pos.y, pos.z);
    }
    void move_s(glm::vec3 v) {
        //printf("    Camera::move_s (sens: %f)\n", sens_move);
        this->move_f(sens_move * v);
    }
    void move(glm::vec3 v, glm::real dt=1.0f) {
        move_s(dt * glm::rotateYl(dir.y) * glm::vec4(v,1.0f));
        //printf("Camera::move (dt: %f)\n", dt);
        //printf("vec: %f %f %f\n", v.x, v.y, v.z);
        //move_s(v * dt);
    }
    void rotate_f(glm::vec3 v) { dir += v; }
    void rotate_s(glm::vec3 v) { rotate_f(v * sens_turn); }
    void rotate(glm::vec3 v, glm::real dt=1.0f) {
        rotate_s(dt * v);
        if(dir.x >=  glm::PI/2) dir.x  =  glm::PI / 2;
        if(dir.x <= -glm::PI/2) dir.x  = -glm::PI / 2;
        if(dir.y >=  glm::PI  ) dir.y -=  glm::PI * 2;
        if(dir.y <= -glm::PI  ) dir.y +=  glm::PI * 2;
        if(dir.z >=  glm::PI  ) dir.z -=  glm::PI * 2;
        if(dir.z <= -glm::PI  ) dir.z +=  glm::PI * 2;
    }
};
#endif // _CAMERA_H_