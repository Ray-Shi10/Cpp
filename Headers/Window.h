#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>
#include <GLM.h>
#include <iostream>
#include <Function.h>

class Window {
public:
    struct windowInfo {
        GLFWwindow* glfwWindow=NULL;
        unsigned int width=-1, height=-1;
        bool pause=false, active;
        glm::vec2 pos;
        windowInfo& operator=(const windowInfo& other) {
            width    = other.width;
            height   = other.height;
            pause    = other.pause;
            active   = other.active;
            pos      = other.pos;
            return *this;
        }
    } window;
    struct mouseInfo {
        bool first = true;
        glm::vec2 last = glm::vec2(0.0f),
                  pos = glm::vec2(0.0f),
                  offset = glm::vec2(0.0f);
        glm::vec2 wheelScl = glm::vec2(0.0f),
                  wheelNow = glm::vec2(0.0f);
        mouseInfo& operator=(const mouseInfo& other) {
            first    = other.first;
            last     = other.last;
            pos      = other.pos;
            offset   = other.offset;
            return *this;
        }
    } mouse;
    struct frameInfo {
        float dt = 0.0f; // delta time
        float lt = 0.0f; // last time
        frameInfo& operator=(const frameInfo& other) {
            dt = other.dt;
            lt = other.lt;
            return *this;
        }
    } frame;

    template <typename _CallBack_t>
    class Callback { public: using _CallBack = function<_CallBack_t>;
        _CallBack callback;
        using _SetCallback_t = function<void(GLFWwindow*, _CallBack_t)>;
        _SetCallback_t setCallBack;
        Callback(_SetCallback_t setCallBack) : setCallBack(setCallBack) {}
        template <typename _Func> Callback(_Func func) { setCallBack = make_function(func); }
        void operator=(_CallBack func) { set(func); }
        template <typename _Func> void operator=(_Func func) { set(make_function(func)); }
        void set(_CallBack func) { callback = func; setCallBack(window.glfwWindow, callback); }
        operator _CallBack() { return callback; }
    };
    Callback<void(GLFWwindow*, int, int)> onFramebufferSize;

    //Window(){}
    Window(unsigned int width, unsigned int height, const char* title, bool active=true)
         :  window  ({glfwCreateWindow(width, height, title, NULL, NULL), width, height, false, active}),
            mouse   ({true, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec2(0.0f)}),
            frame   ({0.0f, 0.0f}),
            onFramebufferSize(glfwSetFramebufferSizeCallback) {
        if(window.glfwWindow == NULL) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            exit(-1);
        } else {
            std::cout << "Created GLFW window successfully\n";
        }
        if(active) {
            glfwMakeContextCurrent(window.glfwWindow);
        }
    }
    ~Window() {
        glfwDestroyWindow(window.glfwWindow);
    }
    operator GLFWwindow*() { return window.glfwWindow; }
    operator const GLFWwindow*() const { return *this; }

    void getFramebufferSizeEvent(int width, int height) {
        window.width  = width;
        window.height = height;
        mouse.first = true;
        if(window.active)
            glViewport(0, 0, width, height);
    }
};

#endif // _WINDOW_H_