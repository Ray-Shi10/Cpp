#ifndef _SHADER_H_
#define _SHADER_H_

#include <vector>
//#include <iostream>
#include <string>
#include "base.h"

class ShaderProgram {
private:
    template <typename _T> using vector = std::vector<_T>;
    using string = std::string;
public:
    using shaderID = GLuint;
    using shaderType = GLuint;
    using shaderSource = const char*;
private:
    static shaderSource getShaderTypeName(shaderType type) {
        switch(type) {
            case GL_VERTEX_SHADER   : return "VERTEX"  ;
            case GL_FRAGMENT_SHADER : return "FRAGMENT";
            case GL_GEOMETRY_SHADER : return "GEOMETRY";
            case GL_PROGRAM         : return "PROGRAM" ;
            default: return "UNKOWN";
        }
    }
    static void checkError(shaderID shader, shaderType type) {
        int success; char infoLog[1024];
        (type==GL_PROGRAM ? glGetProgramiv : glGetShaderiv) (shader, (type==GL_PROGRAM ? GL_LINK_STATUS : GL_COMPILE_STATUS), &success);
        if (!success) {
            (type==GL_PROGRAM ? glGetProgramInfoLog : glGetShaderInfoLog) (shader, 1024, NULL, infoLog);
            std::error << "\nERROR::SHADER::" << (type==GL_PROGRAM?"LINKING":"COMPILATION") << ":  ";
            std::error << getShaderTypeName(type) << "\n    ";
            int i = 0;
            while(infoLog[i]) {
                if(infoLog[i] == '\r') { i++; continue; }
                if(infoLog[i] == '\n') {
                    {//if(*(infoLog+1) != '\n') {
                        std::error << "\n    ";
                    } i++; continue;
                } std::error << infoLog[i]; i++;
            } exit(-1);
        }
    }
    static shaderID compile(shaderSource shaderCode, shaderID type) {
        std::out << "Compiling shader of type " << getShaderTypeName(type) << "...\r";
        //std::out << shaderCode << "\n";
        shaderID shader = glCreateShader(type);
        glShaderSource(shader, 1, &(shaderCode), NULL);
        glCompileShader(shader); checkError(shader, type);
        std::succ << "Compiled shader of type " << getShaderTypeName(type) << " successfully\n";
        return shader;
    }
    void link(vector<shaderID> const&shaders) {
        ID = glCreateProgram();
        for(auto const& shader : shaders) { glAttachShader(ID, shader); }
        glLinkProgram(ID); checkError(ID, GL_PROGRAM);
        for(auto const& shader : shaders) { glDeleteShader(shader); }
    }
    template <typename ...Args>
    static void _Shader(vector<shaderID> &shaders, shaderSource gShaderCode, Args ...otherCodes) {
        shaders.push_back(compile(gShaderCode, GL_GEOMETRY_SHADER));
        _Shader(shaders, otherCodes...);
    }
    static void _Shader(vector<shaderID> &shaders, shaderSource fShaderCode) {
        shaders.push_back(compile(fShaderCode, GL_FRAGMENT_SHADER));
    }
public:
    shaderID ID;
    template <typename ...Args>
    ShaderProgram(shaderSource vShaderCode, Args ...otherCodes) {
        //std::out << "Creating shader program: \n";
        vector<shaderID> shaders(1, compile(vShaderCode, GL_VERTEX_SHADER));
        _Shader(shaders, otherCodes...); link(shaders);
        std::succ << "Shader program created successfully\n";
    }
    ShaderProgram(vector<shaderID> shaders) { link(shaders); }
    void use() const { glUseProgram(ID); } void unuse() const { glUseProgram(0); }
    GLint getUniformLocation(string name) const { return glGetUniformLocation(ID, name.c_str()); }

    template <typename T> void set(string name, T value) const { set(name, (int)value); }
    void set(string name, int value) const { glUniform1i(getUniformLocation(name), value); }
    void set(string name, float value) const { glUniform1f(getUniformLocation(name), value); }
    void set(string name, glm::vec2 const&value) const {
        glUniform2fv(getUniformLocation(name), 1, value_ptr(value));
    }
    void set(string name, glm::vec3 const&value) const {
        glUniform3fv(getUniformLocation(name), 1, value_ptr(value));
    }
    void set(string name, glm::vec4 const&value) const {
        glUniform4fv(getUniformLocation(name), 1, value_ptr(value));
    }
    void set(string name, glm::mat2 const&value) const {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
    void set(string name, glm::mat3 const&value) const {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
    void set(string name, glm::mat4 const&value) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }
};

#endif // _SHADER_H_