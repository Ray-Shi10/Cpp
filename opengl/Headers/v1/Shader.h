#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

//#include <string>
#include <vector>
#include <iostream>
#include <GLM.h>
#include <GLFW/glfw3.h>

class Shader {
private:
    using shaderID = GLuint;
    template <typename _T> using vector = std::vector<_T>;
    static shaderID compile(const char *shaderCode, shaderID type) {
        shaderID shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader); checkError(shader, type);
        return shader;
    }
    static shaderID link(vector<shaderID> const&shaders) {
        shaderID ID = glCreateProgram();
        for(auto const& shader : shaders) { glAttachShader(ID, shader); }
        glLinkProgram(ID); checkError(ID, GL_PROGRAM);
        for(auto const& shader : shaders) { glDeleteShader(shader); }
        return ID;
    }
    template <typename ...Args>
    static void _Shader(vector<shaderID> &shaders, const char *gShaderCode, Args ...otherCodes) {
        shaders.push_back(compile(gShaderCode, GL_GEOMETRY_SHADER));
        _Shader(shaders, otherCodes...);
    }
    static void _Shader(vector<shaderID> &shaders, const char *fShaderCode) {
        shaders.push_back(compile(fShaderCode, GL_FRAGMENT_SHADER));
    }
public:
    shaderID ID;
    template <typename ...Args>
    Shader(const char* vShaderCode, Args ...otherCodes) {
        vector<shaderID> shaders;
        shaders.push_back(compile(vShaderCode, GL_VERTEX_SHADER));
        _Shader(shaders, otherCodes...);
        ID = link(shaders);
    }
    Shader(vector<shaderID> shaders) { link(shaders); }
    void use() const { glUseProgram(ID); } void unuse() const { glUseProgram(0); }
    GLint getUniformLocation(const char* name) const { return glGetUniformLocation(ID, name); }
    template <typename T>
    void set(const char* name, T value) const {
        set(name, (int)value);
    }
    void set(const char* name, int value) const {
        glUniform1i(getUniformLocation(name), value);
    }
    void set(const char* name, bool value) const {
        glUniform1i(getUniformLocation(name), (int)value);
    }
    void set(const char* name, float value) const {
        glUniform1f(getUniformLocation(name), value);
    }
    void set(const char* name, glm::vec2 const& value) const {
        glUniform2fv(getUniformLocation(name), 1, value_ptr(value));
    }
    void set(const char* name, glm::vec3 const& value) const {
        glUniform3fv(getUniformLocation(name), 1, value_ptr(value));
    }
    void set(const char* name, glm::vec4 const& value) const {
        glUniform4fv(getUniformLocation(name), 1, value_ptr(value));
    }
    void set(const char* name, glm::mat2 const& value) const {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
    void set(const char* name, glm::mat3 const& value) const {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
    void set(const char* name, glm::mat4 const& value) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
private:
    static void checkError(shaderID shader, shaderID type) {
        int success; char *infoLog = new char[1024];
        if(type == GL_PROGRAM) {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
        } else {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        }
        if (!success) {
            if(type == GL_PROGRAM) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            } else {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            }
            while(*infoLog == '\n') infoLog++;
            std::error << "\nERROR::SHADER::" << (type==GL_PROGRAM?"LINKING":"COMPILATION") << ":  ";
            switch(type) {
                case GL_VERTEX_SHADER   : std::error << "VERTEX"    ; break;
                case GL_FRAGMENT_SHADER : std::error << "FRAGMENT"  ; break;
                case GL_GEOMETRY_SHADER : std::error << "GEOMETRY"  ; break;
                case GL_PROGRAM         : std::error << "PROGRAM"   ; break;
            } std::error << "\n    ";
            while(*infoLog) {
                //if(*infoLog == '\r') { continue; }
                if(*infoLog == '\n') {
                    if(*(infoLog+1) != '\n') {
                        std::error << "\n    ";
                    } infoLog++; continue;
                } std::error << *infoLog; infoLog++;
            }
            exit(-1);
        }
    }
public:
    static shaderID maxAttribute() {
        int nrAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        return nrAttributes;
    }
    static void logMaxAttribute() {
        //std::out << "Maximum number of vertex attributes supported: " << maxAttribute() << std::endl;
    }
};
#endif