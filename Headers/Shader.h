#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <GLM.h>
#include <GLFW/glfw3.h>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vShaderCode, const char* fShaderCode) {
        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    void use() const { glUseProgram(ID); }
    GLint getUniformLocation(const char* name) const { return glGetUniformLocation(ID, name); }
    template <glm::qualifier Q=glm::qualifier::defaultp, typename T>
    void set(const char* name, T value) const {
        glUniform1i(getUniformLocation(name), (int)value);
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::vec<1, float, Q> const& value) const {
        glUniform3fv(getUniformLocation(name), 1, value_ptr(value));
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::vec<2, float, Q> const& value) const {
        glUniform3fv(getUniformLocation(name), 1, value_ptr(value));
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::vec<3, float, Q> const& value) const {
        glUniform3fv(getUniformLocation(name), 1, value_ptr(value));
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::vec<4, float, Q> const& value) const {
        glUniform3fv(getUniformLocation(name), 1, value_ptr(value));
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::mat<2, 2, float, Q> const& value) const {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::mat<3, 3, float, Q> const& value) const {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
    template <glm::qualifier Q=glm::qualifier::defaultp>
    void set(const char* name, glm::mat<4, 4, float, Q> const& value) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char *infoLog = new char[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                while(*infoLog == '\n') infoLog++;
                std::string infoLogStr(infoLog);
                while(infoLogStr.back() == '\n') infoLogStr.pop_back();
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLogStr.c_str() << "\n -------------------------------------------------------\n";
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                while(*infoLog == '\n') infoLog++;
                std::string infoLogStr(infoLog);
                while(infoLogStr.back() == '\n') infoLogStr.pop_back();
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLogStr.c_str() << "\n -------------------------------------------------------\n";
            }
        }
    }
};
#endif