#ifndef _MY_GLFW_H_
#define _MY_GLFW_H_

#include <GLAD/glad.c>
#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>
#include "Colorful.h"

void initGLFW(short major, short minor) {
    char version[16];
    sprintf(version, "GLFW-v%d.%d", major, minor);
    std::out << "Initializing " << version << " ...\r";
    if(/**/glfwInit()/*/1/**/) {
        std::succ << "Initialized " << version << " successfully" << "\n";
    } else {
        std::error << "Failed to initialize " << version << "\n";
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

GLFWwindow* glfwCreateWindow(int width, int height, const char* title, bool context=true) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    std::out << "Creating GLFW window...\r";
    if (window) {
        std::succ << "Created GLFW window successfully\n";
    } else {
        std::error << "Failed to create GLFW window\n";
        glfwTerminate(); exit(-1);
    }
    if(context) {
        glfwMakeContextCurrent(window);
    }
    return window;
}

size_t size_of(GLenum type, size_t _default = 0) {
    switch(type) {
        case GL_BYTE           : return sizeof(GLbyte)      ;
        case GL_UNSIGNED_BYTE  : return sizeof(GLubyte)     ;
        case GL_SHORT          : return sizeof(GLshort)     ;
        case GL_UNSIGNED_SHORT : return sizeof(GLushort)    ;
        case GL_INT            : return sizeof(GLint)       ;
        case GL_UNSIGNED_INT   : return sizeof(GLuint)      ;
        case GL_FLOAT          : return sizeof(GLfloat)     ;
        case GL_DOUBLE         : return sizeof(GLdouble)    ;
        //case GL_HALF_FLOAT     : return sizeof(GLhalf)      ;
        //case GL_FIXED          : return sizeof(GLfixed)     ;
        default: return _default;
    }
}

template <typename T> class GLTypeInfo { public: using type = void;     constexpr static GLenum value = 0                 ; constexpr static size_t size = 0;                  };
template <> class GLTypeInfo<GLbyte>   { public: using type = GLbyte;   constexpr static GLenum value = GL_BYTE           ; constexpr static size_t size = sizeof(GLbyte);     };
template <> class GLTypeInfo<GLubyte>  { public: using type = GLubyte;  constexpr static GLenum value = GL_UNSIGNED_BYTE  ; constexpr static size_t size = sizeof(GLubyte);    };
template <> class GLTypeInfo<GLshort>  { public: using type = GLshort;  constexpr static GLenum value = GL_SHORT          ; constexpr static size_t size = sizeof(GLshort);    };
template <> class GLTypeInfo<GLushort> { public: using type = GLushort; constexpr static GLenum value = GL_UNSIGNED_SHORT ; constexpr static size_t size = sizeof(GLushort);   };
template <> class GLTypeInfo<GLint>    { public: using type = GLint;    constexpr static GLenum value = GL_INT            ; constexpr static size_t size = sizeof(GLint);      };
template <> class GLTypeInfo<GLuint>   { public: using type = GLuint;   constexpr static GLenum value = GL_UNSIGNED_INT   ; constexpr static size_t size = sizeof(GLuint);     };
template <> class GLTypeInfo<GLfloat>  { public: using type = GLfloat;  constexpr static GLenum value = GL_FLOAT          ; constexpr static size_t size = sizeof(GLfloat);    };
template <> class GLTypeInfo<GLdouble> { public: using type = GLdouble; constexpr static GLenum value = GL_DOUBLE         ; constexpr static size_t size = sizeof(GLdouble);   };
//template <> class GLTypeInfo<GLhalf>   { using type = GLhalf;   constexpr static GLenum value = GL_HALF_FLOAT     ; constexpr static size_t size = sizeof(GLhalf);     };
//template <> class GLTypeInfo<GLfixed>  { using type = GLfixed;  constexpr static GLenum value = GL_FIXED          ; constexpr static size_t size = sizeof(GLfixed);    };

#endif // _MY_GLFW_H_