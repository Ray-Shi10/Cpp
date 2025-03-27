#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

//#include <iostream>
#include <vector>
#include <string>
//#include <initializer_list>
#include "base.h"
#include "Buffer.h"

class VertexArray {
private:
    using size_t = std::size_t;
    template <typename T>
    using vector = std::vector<T>;
    using string = std::string;
    template <typename ...Args>
    using tuple = std::tuple<Args...>;
public:
    static VertexArray *bound;
    using BufferID = GLuint;
    BufferID id;
    bool binded = 0;
    vector<BufferID> buffers;
    GLuint attribIndex = 0;
     VertexArray() { glGenVertexArrays   (1, &id); }
    ~VertexArray() { glDeleteVertexArrays(1, &id);
        for(auto const& buffer : buffers) {
            glDeleteBuffers(1, &buffer);
        }
    }
    void bind() {
        if(bound != this) {
            if(bound) {
                bound -> binded = false;
            }
            binded = true;
            glBindVertexArray(id);
            bound = this;
        }
    }
    void unbind() {
        if(bound == this) {
            bound = nullptr;
            binded = false;
            glBindVertexArray(0);
        }
    }
    static void clear() {
        if(bound) bound -> unbind();
    }
    template <typename T>
    void operator()(vector<T> const&data, glm::length_t n=1, glm::length_t m=1) {
        bind(); BufferID buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
            for(glm::length_t i=0; i<m; i++) {
                glVertexAttribPointer(attribIndex, n, GL_FLOAT, GL_FALSE, n * sizeof(T), (void*)(i * n * sizeof(T)));
                glEnableVertexAttribArray(attribIndex++);
            }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        buffers.push_back(buffer);
    }
};
VertexArray *VertexArray::bound = nullptr;

#endif // _VERTEX_ARRAY_H_