#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "base.h"
#include <v1/Function.h>

class Window {
public:
    struct WindowData {
        GLFWwindow *glfwWindow = nullptr;
        const char *title;
        glm::uvec2 size;
        glm::vec2 pos;
        bool active;
        glm::real aspect() { return glm::real(size.x) / glm::real(size.y); }
    } window;
    struct FrameData {
        glm::real dt, nt;
        glm::real lt;
    } frame;
    struct MouseData {
        GLenum mode = GLFW_CURSOR_NORMAL;
        bool entering = true;
        glm::vec2 last, pos, offset, wheelScl, wheelNow;
    } mouse;
    static bool __first__;

    Window() = default;
    Window(const char *title,                                 bool active=true) : Window(title, glm::uvec2(0), glm::vec2(0), active) { }
    Window(const char *title,                  glm::vec2 pos, bool active=true) : Window(title, glm::vec2(0), pos, active) { }
    Window(const char *title, glm::uvec2 size,                bool active=true) : Window(title, size, glm::vec2(0), active) { }
    Window(const char *title, glm::uvec2 size, glm::vec2 pos, bool active=true) : 
        window({nullptr, title, size, pos, active}), 
        frame({0, glm::real(glfwGetTime())}), 
        mouse({GLFW_CURSOR_NORMAL, true, glm::vec2(0), glm::vec2(0), glm::vec2(0), glm::vec2(0)}) {
        window.glfwWindow = glfwCreateWindow(window.size.x, window.size.y, window.title, nullptr, nullptr);
        if(active) {
            glfwMakeContextCurrent(window.glfwWindow);
            glfwSetWindowPos(window.glfwWindow, window.pos.x, window.pos.y);
            if(__first__) {
                initGLAD(); __first__ = false;
                glfwSetErrorCallback([](int error, const char* description) {
                    std::error << "GLFW-ERROR(" << error << "):  " << description << std::endl;
                });
            }
        }//glfwGetCurrentContext();
    }
    operator GLFWwindow*() { return window.glfwWindow; }
    operator const GLFWwindow*() const { return *this; }

    bool shouldClose() { return !glfwWindowShouldClose(window.glfwWindow); }
    void close(bool close=true) { glfwSetWindowShouldClose(window.glfwWindow, close); }
    void swapBuffers() { glfwSwapBuffers(window.glfwWindow); }
    void clearDevice(glm::vec3 color, glm::real alpha=1.0f) { clearDevice(glm::vec4(color,alpha)); }
    void clearDevice(glm::real gray, glm::real alpha=1.0f) { clearDevice(glm::vec3(gray), alpha); }
    void clearDevice(glm::vec4 color) { glClearColor(color.r, color.g, color.b, color.a); glClear(GL_COLOR_BUFFER_BIT); }
    void clearDepth() { glClear(GL_DEPTH_BUFFER_BIT); }
    void clearStencil() { glClear(GL_STENCIL_BUFFER_BIT); }
    bool keyPressed(int key) { return glfwGetKey(window.glfwWindow, key) == GLFW_PRESS; }
    void setCursorMode(int mode) { mouse.mode = mode; }
    void applyCursorMode() { if(window.active) { glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, mouse.mode); } else { glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); } }
    void newFrame(float newFrame = glfwGetTime()) {
        frame.lt=frame.nt; frame.nt=newFrame; frame.dt = frame.nt - frame.lt;
        while(!window.active && !shouldClose()) {
            glfwWaitEvents();
        }
    }
};
bool Window::__first__ = true;

#endif // _WINDOW_H_