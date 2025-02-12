#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GL.h>
#include <Function.h>

class Window {
public:
    struct windowInfo {
        GLFWwindow* glfwWindow=NULL;
        unsigned int width=-1, height=-1;
        bool active;
        glm::vec2 pos;
        float aspect() { return float(width/height); }
        windowInfo& operator=(const windowInfo& other) {
            width    = other.width;
            height   = other.height;
            active   = other.active;
            pos      = other.pos;
            return *this;
        }
    } window;
    struct mouseInfo {
        int mode = GLFW_CURSOR_NORMAL;
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
    class Callback { public: using _CallBack = function_t<_CallBack_t>; _CallBack callback;
        using _SetCallback_t = function_t<void(_CallBack_t)>; _SetCallback_t const setCallBack;
        template <typename _Func> Callback(_Func func) : setCallBack(function(func)) { }
        template <typename _Func> void operator=(_Func func) { callback=func; setCallBack(callback); }
        operator _CallBack() { return callback; }
    };
    Callback<void(GLFWwindow*, int, int)> onSize;
    Callback<void(GLFWwindow*, double, double)> onCursorMove;
    Callback<void(GLFWwindow*, int, int, int, int)> onKey;
    Callback<void(GLFWwindow*, int)> onFocus;

    Window(const char* title, bool active=true) : Window(1,1,title,active) {}
    Window(unsigned int width, unsigned int height, const char* title, bool active=true)
         :  window  ({glfwCreateWindow(width, height, title), width, height, active}),
            mouse   ({GLFW_CURSOR_NORMAL, true, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec2(0.0f)}),
            frame   ({0.0f, 0.0f}),
            onSize([&](void(*callback)(GLFWwindow*, int, int)){
                glfwSetFramebufferSizeCallback(window.glfwWindow, callback);
            }), 
            onCursorMove([&](void(*callback)(GLFWwindow*, double, double)){
                glfwSetCursorPosCallback(window.glfwWindow, callback);
            }), 
            onKey([&](void(*callback)(GLFWwindow*, int, int, int, int)){
                glfwSetKeyCallback(window.glfwWindow, callback);
            }), 
            onFocus([&](void(*callback)(GLFWwindow*, int)){
                glfwSetWindowFocusCallback(window.glfwWindow, callback);
            }) {
        if(__firstWindow) {
            __firstWindow = false;
            glfwSetErrorCallback([](int error, const char* description) {
                printf("GLFW-ERROR(%08d):  %s", error, description);
            });
            initGLAD();
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

    void setCursorMode(int mode) {
        mouse.mode = mode;
    }
    void applyCursorMode(signed char apply=-1) {
        if(apply < 0) {
            apply = window.active;
        }
        if(apply) {
            if(glfwGetInputMode(window.glfwWindow, GLFW_CURSOR) != mouse.mode) {
                glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, mouse.mode);
            }
        } else {
            if(glfwGetInputMode(window.glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
                glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
    void newFrame(float time=glfwGetTime()) {
        frame.dt = time - frame.lt; frame.lt = time;
    }
    bool keyPress(int key) {
        return glfwGetKey(window.glfwWindow, key) == GLFW_PRESS;
    }

    void getFramebufferSizeEvent(int width, int height, bool apply=true) {
        window.width  = width;
        window.height = height;
        mouse.first = true;
        if(apply)
        {
            glViewport(0, 0, width, height);
        }
    }
    void getCursorMoveEvent(double xpos, double ypos) {
        if(mouse.first) {
            mouse.last = glm::vec2(xpos, ypos);
            mouse.first = false;
        }
        mouse.pos = glm::vec2(xpos, ypos);
        mouse.offset = mouse.pos - mouse.last;
        mouse.last = mouse.pos;
    }
    void getKeyEvent(int key, int scancode, int action, int mods) {
    }
    void getFocusEvent(int focused) {
    }

private:
    bool __firstWindow = true;
};

#endif // _WINDOW_H_