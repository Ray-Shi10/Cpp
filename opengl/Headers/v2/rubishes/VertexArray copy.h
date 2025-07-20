#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

//#include <iostream>
#include <vector>
#include <string>
//#include <initializer_list>
#include "base.h"
#include "TypeTraits.h"

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
    bool binded;
    VertexArray() {
        glGenVertexArrays(1, &id);
    }
    ~VertexArray() {
        glDeleteVertexArrays(1, &id);
    }
    void bind() {
        if(bound != this) {
            bound -> binded = false;
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
    class _TMP {
    private:
        static GLuint readUNum(string &str) { GLuint num = 0, i=0;
            while(i<str.size() && isdigit(str[i])) {
                num = num * 10 + str[0] - '0'; i++;
            } str = str.substr(i);
            return num;
        }
    public:
        VertexArray *vao = nullptr;
        GLuint attIndex = 0;
        GLuint attOffset;
        vector<BufferID> buffers;
        _TMP(VertexArray *vao) : vao(vao) {}
        ~_TMP() { vao -> unbind(); }/*
        template <typename T>
        _TMP& operator()(vector<vector<T>> data, vector<vector<string>> atts) {
            BufferID buffer; glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            size_t dataSize = 0;
            for(size_t i = 0; i < data.size(); i++) {
                dataSize += data[i].size() * sizeof(T);
            } glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
            size_t offset = 0;
            for(size_t i = 0; i < data.size(); i++) {
                glBufferSubData(GL_ARRAY_BUFFER, offset, data[i].size() * sizeof(T), data[i].data());
                offset += data[i].size() * sizeof(T);
            } buffers.push_back(buffer);
            for(size_t i = 0; i < atts.size(); i++) {
                GLuint stride = 0;
                vector<std::pair<GLuint, GLenum>> attFmts;
                for(string const& att : atts[i]) {
                    //while(att.size() && isspace(att[0]      )) att = att.substr(1);
                    //while(att.size() && isspace(att.back()  )) att.pop_back();
                    GLuint amount = readUNum(att), size;
                    GLenum type = GL_FLOAT;
                    string head = att.substr(0, 3);
                    att = att.substr(3);
                    if(head == "vec" || head == "col") {
                        size = readUNum(att);
                    } else if(head == "mat") {
                        size = readUNum(att);
                        GLuint size2 = size;
                        if(att[0] == 'x') {
                            size2 = readUNum(att=att.substr(1));
                        }
                        amount *= size2;
                    }
                    if(att.size()) {
                        switch(att[0]) {
                            case 'b': type = GL_BOOL;           break;
                            case 'B': type = GL_BYTE;           break;
                            case 's': type = GL_SHORT;          break;
                            case 'i': type = GL_INT;            break;
                            case 'u': type = GL_UNSIGNED_INT;   break;
                            case 'f': type = GL_FLOAT;          break;
                            case 'd': type = GL_DOUBLE;         break;
                            default: std::error << "ERROR::VERTEX_ARRAY:: Unknown type: " << att[0] << std::endl; exit(-1);
                        }
                    }
                    for(GLuint j = 0; j < amount; j++) {
                        attFmts.push_back({size, type});
                    }
                    stride += size * size_of(type);
                }
                for(auto const& att : attFmts) {
                    glEnableVertexAttribArray(attIndex);
                    glVertexAttribPointer(attIndex, att.first, att.second, GL_FALSE, (void*)stride);
                    stride += att.first * sizeof(att.second);
                    attIndex++;
                }
            }
            for(auto const& att : attFmts) {
                glEnableVertexAttribArray(att.first);
                glVertexAttribPointer(att.first, size, type, GL_FALSE, stride, (void*)0);
                stride += size * sizeof(type);
            }
            return *this;
        }
        */
        template <typename T>
        _TMP& operator()(vector<T> data, GLuint divisor = 0) {
            this -> _addBuffer(*((vector<tuple<T>>*)&data), &divisor);
            return *this;
        }
    private:
        template <typename ...Tps>
        void _addBuffer(vector<tuple<Tps...>> data, GLuint *divisors) {
            using attPack = tuple<Tps...>;
            BufferID buffer; glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(attPack), data.data(), GL_STATIC_DRAW);
            attOffset = 0;
            _addAttributes<attPack, sizeof(attPack)>(attOffset, divisors);
        }
        template <typename attPack, size_t stride, size_t I=0>
        void _addAttributes(GLuint *divisors) {
            glEnableVertexAttribArray(attIndex);
            _addAttribute<std::tuple_element_t<I,attPack>, stride>(divisors[I]);
            _addAttributes<attPack, stride, I+1>(divisors);
        }
        template <typename T, size_t stride>
        void _addAttribute(GLuint divisor) {
            using solved = glm::solveVecOrMat<T>;
            for(int i=0; i<solved::size2; i++) {
                glVertexAttribPointer(attIndex, solved::size1, GLenumType<solved::type>::value, GL_FALSE, stride, (void*)(attOffset));
            }
            attOffset += sizeof(T);
        }
    };
    _TMP operator()() { bind();
        return _TMP(this);
    }
    template <typename T>
    _TMP operator()(vector<T> data) {
        return (*this)(data.data(), data.size());
    }
    template <typename T>
    _TMP operator()(T *data, GLuint size) { bind();
        BufferID eleBuffer; glGenBuffers(1, &eleBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(T), data, GL_STATIC_DRAW);
        return (*this)();
    }
};

#endif // _VERTEX_ARRAY_H_