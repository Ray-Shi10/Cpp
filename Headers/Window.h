#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>
#include <GLM.h>
#include <iostream>

class Window {
public:
    struct windowInfo {
        GLFWwindow* glfwWindow=NULL;
        int width=-1, height=-1;
        bool pause=false, active;
        glm::ivec2 pos;
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
        float nt = 0.0f; // now time
        frameInfo& operator=(const frameInfo& other) {
            dt = other.dt;
            lt = other.lt;
            nt = other.nt;
            return *this;
        }
    } frame;

    Window(){}
    Window(int width, int height, const char* title, bool active=true)
         :  window  ({glfwCreateWindow(width, height, title, NULL, NULL), width, height, false, active}),
            mouse   ({true, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec2(0.0f)}),
            frame   ({0.0f, 0.0f, 0.0f}) {
        if(window.glfwWindow == NULL) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            exit(-1);
        } else {
            std::cout << "Created GLFW window successfully\n";
        }
        glfwSetWindowUserPointer(window.glfwWindow, this);
        if(active) {
            glfwMakeContextCurrent(window.glfwWindow);
        }
    }
    ~Window() {
        glfwDestroyWindow(window.glfwWindow);
    }
    Window(Window const& other) {
        window = other.window;
        mouse  = other.mouse;
        frame  = other.frame;
    }
    Window(Window&& other) {
        window = other.window;
        mouse  = other.mouse;
        frame  = other.frame;
    }

    glm::vec2 getCenter() const {
        return glm::vec2(window.width/2, window.height/2);
    }
    GLFWwindow* getGLFWwindow() const {
        return window.glfwWindow;
    }

    glm::vec2 getMousePos() const {
        double xpos, ypos;
        glfwGetCursorPos(window.glfwWindow, &xpos, &ypos);
        return glm::vec2(xpos, ypos);
    }
    glm::vec2 getMousePos(glm::vec2 pos) const {
        return glm::vec2(pos.x, window.height-pos.y);
    }
    glm::vec2 checkMousePos() {
        glm::vec2 pos = getMousePos();
        if(glm::lengthSQ(mouse.pos-pos) > glm::epsilon<float>()) {
            mouse.first = true;
            getEvent_MouseMove(pos);
        }
        return pos;
    }
    void setMousePos(glm::vec2 const& pos) {
        getEvent_MouseMove(pos);
        glfwSetCursorPos(window.glfwWindow, pos.x, pos.y);
    }
    void getEvent_MouseMove(glm::vec2 const& nowpos) {
        if (mouse.first) {
            mouse.last = nowpos;
            mouse.first = false;
        }
        mouse.last = mouse.pos;
        mouse.offset = nowpos - mouse.last;
        mouse.pos = nowpos;
    }
    template <typename _Func>
    void setCallback_MouseMove(_Func func) {
        static auto f = func ? func : _DefaultCallback;
        static auto self = *this;
        glfwSetCursorPosCallback(window.glfwWindow, [](GLFWwindow*, double xpos, double ypos) {
            //Window &window = *((Window*)(glfwGetWindowUserPointer(glfwWindow)));
            self.getEvent_MouseMove(glm::vec2(xpos, ypos));
            f(self, self.mouse.pos);
        });
    }
    void getEvent_MouseScroll(glm::vec2 const& scl) {
        mouse.wheelScl = scl;
        mouse.wheelNow += mouse.wheelScl;
    }
    template <typename _Func>
    void setCallback_Scroll(_Func func) {
        static auto f = func ? func : _DefaultCallback;
        static auto self = *this;
        glfwSetScrollCallback(window.glfwWindow, [](GLFWwindow*, double xoffset, double yoffset) {
            //Window &window = *((Window*)(glfwGetWindowUserPointer(glfwWindow)));
            self.getEvent_MouseScroll(glm::vec2(xoffset, -yoffset));
            f(self, self.mouse.wheelScl);
        });
    }

    void getEvent_Focus(int focused) {
        window.active = focused;
    }
    template <typename _Func>
    void setCallback_Focus(_Func func) {
        static auto f = func ? func : _DefaultCallback;
        static auto self = *this;
        glfwSetWindowFocusCallback(window.glfwWindow, [](GLFWwindow*, int focused) {
            f(self, focused);
        });
    }

    void moveto(glm::vec2 const& pos) {
        glfwSetWindowPos(window.glfwWindow, pos.x, pos.y);
    }

    operator GLFWwindow*() const { return window.glfwWindow; }

private:
    template <typename ..._Args>
    static void _DefaultCallback(_Args...){}
};

#endif // _WINDOW_H_