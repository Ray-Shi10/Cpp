#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <GLM.h>
//#include <Input.h>

class Camera {
public:
    glm::vec3 rot, pos, view;
    float sens_turn, sens_move, sens_zoom;
    float znear, zfar;
    Camera(glm::vec3 pos, glm::vec3 rot, glm::vec3 view, glm::real znear, glm::real zfar, glm::real sense_move, glm::real sens_turn, glm::real sens_zoom) : 
        pos(pos), rot(rot), view(view), znear(znear), zfar(zfar), sens_move(sens_move), sens_turn(sens_turn), sens_zoom(sens_zoom) {}
    glm::mat4 getViewMatrix() const {
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::rotate(view, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, -pos);
        return view;
    }
    glm::mat4 getProjectionMatrix() const {
        return glm::mat4(
            view.z/view.x, 0.0f, 0.0f, 0.0f,
            0.0f, view.z/view.y, 0.0f, 0.0f,
            0.0f, 0.0f, (zfar+znear)/(zfar-znear), -(2*zfar*znear)/(zfar-znear),
            0.0f, 0.0f, 1.0f, 0.0f
        );
    }
    glm::mat4 getMatrix() const {
        return getProjectionMatrix() * getViewMatrix();
    }
    void move_f(glm::vec3 v) { pos += v; }
    void move_s(glm::vec3 v) { move_f(v * sens_move); }
    void move(glm::vec3 v, glm::real dt=1.0f) {
        move_s(dt * glm::rotateXl(rot.x) * glm::rotateYl(rot.y) * glm::rotateZl(rot.z) * glm::vec4(v,1.0f));
    }
    void rotate_f(glm::vec3 v) { rot += v; }
    void rotate_s(glm::vec3 v) { rotate_f(v * sens_turn); }
    void rotate(glm::vec3 v, glm::real dt=1.0f) {
        rotate_s(dt * v);
        if(rot.x >=  glm::PI/2) rot.x  =  glm::PI / 2;
        if(rot.x <= -glm::PI/2) rot.x  = -glm::PI / 2;
        if(rot.y >=  glm::PI  ) rot.y -=  glm::PI * 2;
        if(rot.y <= -glm::PI  ) rot.y +=  glm::PI * 2;
        if(rot.z >=  glm::PI  ) rot.z -=  glm::PI * 2;
        if(rot.z <= -glm::PI  ) rot.z +=  glm::PI * 2;
    }
};
#endif // _CAMERA_H_