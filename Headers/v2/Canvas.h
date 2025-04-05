#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "GLM.h"

class Canvas_base {
public:
    glm::ivec2 size, pos;
    glm::vec2 scale, offset;
    
};

#endif // _CANVAS_H_