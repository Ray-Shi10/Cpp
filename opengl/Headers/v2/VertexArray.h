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
    GLenum eboType = 0;
    static VertexArray *getBound() { return bound; }
     VertexArray() { glGenVertexArrays   (1, &id); }
    ~VertexArray() { glDeleteVertexArrays(1, &id);
        for(auto const& buffer : buffers) {
            glDeleteBuffers(1, &buffer);
        }
    }
    void bind() {
        if(bound != this) {
            if(bound)
                bound -> binded = false;
            binded = true;
            glBindVertexArray(id);
            nowEBOType = eboType;
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
private:
    template <typename buffer_t, size_t index=buffer_t::packLen, size_t offset=0>
    static void _setVertAttrib(buffer_t const&buffer, GLuint divisor, GLuint &attribIndex) {
        using type = typename buffer_t::kthType<index>::type;
        using typeInfoGLM = glm::solveType<type>;
        using typeInfoGL  = GLTypeInfo<typename typeInfoGLM::type>;
        //std::cout << typeInfoGLM::n << " " << typeInfoGLM::m << " " << typeInfoGLM::size << " " << typeInfoGL::value << " " << buffer_t::packSize << " " << offset << "\n";
        for(glm::length_t i=0; i<typeInfoGLM::m; i++) {
            glVertexAttribPointer(attribIndex, typeInfoGLM::n, typeInfoGL::value, GL_FALSE, buffer_t::packSize, (void const*)offset);
            glVertexAttribDivisor(attribIndex, divisor);
            glEnableVertexAttribArray(attribIndex);
            attribIndex++;
        }
    }
    template <typename buffer_t, size_t index=buffer_t::packLen-1, size_t offset=0>
    struct setVertAttrib {
        setVertAttrib(buffer_t const&buffer, GLuint divisor, GLuint &attribIndex) {
            _setVertAttrib<buffer_t, index, offset>(buffer, divisor, attribIndex);
            setVertAttrib<buffer_t, index-1, offset-sizeof(typename buffer_t::kthType<index>::type)>(buffer, divisor, attribIndex);
        }
    };
    template <typename buffer_t, size_t offset>
    struct setVertAttrib<buffer_t, 0, offset> {
        setVertAttrib(buffer_t const&buffer, GLuint divisor, GLuint &attribIndex) {
            _setVertAttrib<buffer_t, 0, offset>(buffer, divisor, attribIndex);
        }
    };
public:
    template <typename ...Args>
    VertexArray &operator()(Buffer<Args...> const&data, GLuint divisor=0) {
        using buffer_t = Buffer<Args...>;
        bind(); BufferID buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, data.sizeByte(), data.dataPtr(), GL_STATIC_DRAW);
            setVertAttrib<buffer_t>(data, divisor, attribIndex);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        buffers.push_back(buffer);
        return *this;
    }
    template <typename T>
    VertexArray &operator[](Buffer<T> const&data) {
        eboType = GLTypeInfo<T>::value;
        bind(); BufferID buffer;
        glGenBuffers(1, &buffer);
        //element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.sizeByte(), data.dataPtr(), GL_STATIC_DRAW);
        return *this;
    }
    static GLenum nowEBOType;
};
VertexArray *VertexArray::bound = nullptr;
GLenum VertexArray::nowEBOType = 0;
#undef glDrawElements
inline void glDrawElements(GLenum mode, GLsizei count, void *indices=0) {
    glad_glDrawElements(mode, count, VertexArray::nowEBOType, indices);
}

#endif // _VERTEX_ARRAY_H_