#ifndef _GL_H_
#define _GL_H_

#include <iostream>
//namespace GLAD {
#include <GLAD/glad.c>
//}
#include <GLFW/glfw3.h>
#include <GLM.h>

#define DEBUG_LOG_VALUE(value, connect) #value << connect << value

namespace std {
    static void setColor(ostream &stream, int r, int g, int b) { stream << "\033[38;2;" << r << ";" << g << ";" << b << "m"; }
    static void setColor(ostream &stream, int gray) { stream << "\033[38;2;" << gray << ";" << gray << ";" << gray << "m"; }
    static void setColor(ostream &stream) { stream << "\033[0m"; }
    class color_ostream {
        void init() { sprintf(setColor, "\033[38;2;%03d;%03d;%03dm", r, g, b); }
        template <typename T> void print(T value) { stream << setColor << value << "\033[0m"; }
    public:
        using byte = unsigned char; byte r, g, b; ostream &stream; char setColor[32];
        color_ostream(ostream &stream, byte r, byte g, byte b) : stream(stream), r(r), g(g), b(b) { init(); }
        color_ostream(ostream &stream, byte gray) : color_ostream(stream, gray, gray, gray) { init(); }
        template <typename T> color_ostream &operator<<(T value) { print(value); return *this; }
        color_ostream operator<<(color_ostream const&stream) { return color_ostream(this->stream, stream.r, stream.g, stream.b); }
        color_ostream operator<<(ostream &stream) { return color_ostream(stream, r, g, b); }
        color_ostream &operator<<(ostream&(*func)(ostream&)) { stream << setColor << func << "\033[0m"; return *this; }
        operator ostream&() { return stream; } operator ostream const&() const { return stream; } operator bool() { return true; }
        //template <typename T> using _print_func = void(*)(T); template <typename T> operator _print_func<T>() { return [](T value){(*this)<<vaule;}; }
    } error(cerr, 200,0,0), warn(cerr, 200,64,32), succ(cout, 32,200,32), out(cout, 200);
}

void initGLFW(short major, short minor) {
    if(!glfwInit()) {
        std::error << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    } else {
        std::succ << "Initialized GLFW successfully" << std::endl;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    //return true;
}
void initConsole() { HANDLE stdHandle; DWORD dwMode;
    stdHandle = GetStdHandle(STD_INPUT_HANDLE ); GetConsoleMode(stdHandle, &dwMode); dwMode |= 0x0200; SetConsoleMode(stdHandle, dwMode);
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE); GetConsoleMode(stdHandle, &dwMode); dwMode |= 0x0004; SetConsoleMode(stdHandle, dwMode);
}
void init(short major, short minor) {
    initConsole();
    std::out << std::error << "Error " << std::warn << "Warning " << std::succ << "Success " << std::out << "Normal\n";
    initGLFW(major, minor);
}

GLFWwindow* glfwCreateWindow(int width, int height, const char* title, bool context=true) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        std::error << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(-1);
    } else {
        std::succ << "Created GLFW window successfully\n";
    }
    if(context) {
        glfwMakeContextCurrent(window);
    }
    return window;
}

void initGLAD() {
    if(!//*
        gladLoadGL()/*/
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)//*/
        ) {
        std::error << "Failed to initialize GLAD\n";
        exit(-1);
    } else {
        std::succ << "Initialized GLAD successfully\n";
    }
}
/*
void APIENTRY glDebugOutput(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar *message, 
    void *userParam) {
    // 忽略一些不重要的错误/警告代码
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 
    std::out << "---------------" << std::endl;
    std::out << "Debug message (" << id << "): " <<  message << std::endl;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::out << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::out << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::out << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::out << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::out << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::out << "Source: Other"; break;
    } std::out << std::endl;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::out << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::out << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::out << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::out << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::out << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::out << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::out << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::out << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::out << "Type: Other"; break;
    } std::out << std::endl;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::out << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::out << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::out << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::out << "Severity: notification"; break;
    } std::out << std::endl;
    std::out << std::endl;
}
void initErrorCallback() {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glfwSetErrorCallback([](int error, const char* description) {
            std::error << "Error " << error << ": " << description << std::endl;
        });
    }
}//*/

void gl_Viewport(int width, int height) {
    glViewport(0, 0, width, height);
}

void glfwSetCursor(GLFWwindow* window, int value) {
    glfwSetInputMode(window, GLFW_CURSOR, value);
}
/*
glm::vec2 glfwGetCursorPos(GLFWwindow* window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}*/

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
        case GL_HALF_FLOAT     : return sizeof(GLhalf)      ;
        case GL_FIXED          : return sizeof(GLfixed)     ;
        default: return _default;
    }
}

template <typename T> struct GLenumType { using type = T;        constexpr static GLenum value = 0                 ; constexpr static size_t size = sizeof(T);          };
template <> struct GLenumType<GLbyte>   { using type = GLbyte;   constexpr static GLenum value = GL_BYTE           ; constexpr static size_t size = sizeof(GLbyte);     };
template <> struct GLenumType<GLubyte>  { using type = GLubyte;  constexpr static GLenum value = GL_UNSIGNED_BYTE  ; constexpr static size_t size = sizeof(GLubyte);    };
template <> struct GLenumType<GLshort>  { using type = GLshort;  constexpr static GLenum value = GL_SHORT          ; constexpr static size_t size = sizeof(GLshort);    };
template <> struct GLenumType<GLushort> { using type = GLushort; constexpr static GLenum value = GL_UNSIGNED_SHORT ; constexpr static size_t size = sizeof(GLushort);   };
template <> struct GLenumType<GLint>    { using type = GLint;    constexpr static GLenum value = GL_INT            ; constexpr static size_t size = sizeof(GLint);      };
template <> struct GLenumType<GLuint>   { using type = GLuint;   constexpr static GLenum value = GL_UNSIGNED_INT   ; constexpr static size_t size = sizeof(GLuint);     };
template <> struct GLenumType<GLfloat>  { using type = GLfloat;  constexpr static GLenum value = GL_FLOAT          ; constexpr static size_t size = sizeof(GLfloat);    };
template <> struct GLenumType<GLdouble> { using type = GLdouble; constexpr static GLenum value = GL_DOUBLE         ; constexpr static size_t size = sizeof(GLdouble);   };
//template <> struct GLenumType<GLhalf>   { using type = GLhalf;   constexpr static GLenum value = GL_HALF_FLOAT     ; constexpr static size_t size = sizeof(GLhalf);     };
//template <> struct GLenumType<GLfixed>  { using type = GLfixed;  constexpr static GLenum value = GL_FIXED          ; constexpr static size_t size = sizeof(GLfixed);    };

#endif // _GL_H_