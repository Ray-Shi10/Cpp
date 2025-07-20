#ifndef _ERROR_H_
#define _ERROR_H_

#include "base.h"

void _glCheckError(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        } std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
}
#define glCheckError() _glCheckError(__FILE__, __LINE__)

void initError() {
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        std::succ << "OpenGL debug context enabled\n";
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glGetError(); // clear error
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
        std::out << "Setting OpenGL debug callback\r";
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
            if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
            std::error << "GL-ERROR(" << id << "): " <<  message << "\n";
            std::error << "    Source(" << source << "): ";
            switch(source) {
                case GL_DEBUG_SOURCE_API:             std::error << "API"; break;
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::error << "Window System"; break;
                case GL_DEBUG_SOURCE_SHADER_COMPILER: std::error << "Shader Compiler"; break;
                case GL_DEBUG_SOURCE_THIRD_PARTY:     std::error << "Third Party"; break;
                case GL_DEBUG_SOURCE_APPLICATION:     std::error << "Application"; break;
                case GL_DEBUG_SOURCE_OTHER:           std::error << "Other"; break;
                default:                              std::error << "Unknown"; break;
            } std::error << "\n";
            std::error << "    Type(" << type << "): ";
            switch (type) {
                case GL_DEBUG_TYPE_ERROR:               std::error << "Error"; break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::error << "Deprecated Behaviour"; break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::error << "Undefined Behaviour"; break; 
                case GL_DEBUG_TYPE_PORTABILITY:         std::error << "Portability"; break;
                case GL_DEBUG_TYPE_PERFORMANCE:         std::error << "Performance"; break;
                case GL_DEBUG_TYPE_MARKER:              std::error << "Marker"; break;
                case GL_DEBUG_TYPE_PUSH_GROUP:          std::error << "Push Group"; break;
                case GL_DEBUG_TYPE_POP_GROUP:           std::error << "Pop Group"; break;
                case GL_DEBUG_TYPE_OTHER:               std::error << "Other"; break;
                default:                                std::error << "Unknown"; break;
            } std::error << "\n";
            std::error << "    Severity(" << severity << "): ";
            switch (severity) {
                case GL_DEBUG_SEVERITY_HIGH:         std::error << "Severity: high"; break;
                case GL_DEBUG_SEVERITY_MEDIUM:       std::error << "Severity: medium"; break;
                case GL_DEBUG_SEVERITY_LOW:          std::error << "Severity: low"; break;
                case GL_DEBUG_SEVERITY_NOTIFICATION: std::error << "Severity: notification"; break;
                default:                             std::error << "Unknown"; break;
            } std::error << "\n";
        }, nullptr);
        std::succ << "Setting OpenGL debug callback successfully\n";
    } else {
        std::error << "OpenGL debug context not enabled\n";
    }
    glfwSetErrorCallback([](int error, const char* description) {
        std::error << "GLFW-ERROR(" << error << "):  " << description << "\n";
    });
}

#endif // _ERROR_H_