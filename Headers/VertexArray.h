#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

#include <GL.h>
#include <string>
#include <vector>

class VertexArray {
private:
    static VertexArray *_nowVAO;
    typedef unsigned int VBufferID;
    template <typename _T>
    using vector = std::vector<_T>;
    using string = std::string;
    unsigned short _buffId = 0;
public:
    unsigned int VAO; bool binded = false;
    struct VertexBuffer {
        unsigned int VBO;
        VertexBuffer() { glGenBuffers(1, &VBO); }
        operator unsigned int() { return VBO; }
        operator const unsigned int() const { return VBO; }
    }; vector<VertexBuffer> VBOs;
    struct VB_data {
        void *data; unsigned int size;
    };
    struct Attribute {
        unsigned short size; GLenum type; unsigned short divisor;
    };
    struct Buffer {
        struct BuffData : VB_data {
            vector<Attribute> attribs;
        }; vector<BuffData> buffs;
        VB_data *element = nullptr;
    }; vector<Buffer> buffers;
    VertexArray() : buffers(1) { glGenVertexArrays(1, &VAO); }
    ~VertexArray() { glDeleteVertexArrays(1, &VAO);
        for(VBufferID VBO : VBOs) { glDeleteBuffers(1, &VBO); }
    }
    operator unsigned int() { return VAO; }
    operator const unsigned int() const { return VAO; }
    void bind() {
        if(_nowVAO != this) {
            if(_nowVAO) {
                _nowVAO->binded = false;
            }
            binded = true;
            glBindVertexArray(VAO);
            _nowVAO = this;
        }
    }
    void unbind() {
        if(_nowVAO == this) {
            binded = false;
            glBindVertexArray(0);
            _nowVAO = nullptr;
        }
    }
    static void clear() {
        if(_nowVAO) {
            _nowVAO->binded = false;
            glBindVertexArray(0);
            _nowVAO = nullptr;
        }
    }
    template <typename _Buffer_t>
    void addBuffer(_Buffer_t *buffer, unsigned int size) {
        if(_buffId) {
            _buffId = 0;
            buffers.push_back(Buffer());
        }
        buffers.back().buffs.push_back({(void*)buffer, size});
    }
    template <typename _Buffer_t>
    void addBuffer(vector<_Buffer_t> const& buffer) {
        addBuffer(buffer.data(), buffer.size()*sizeof(_Buffer_t));
    }
    template <typename _Buffer_t>
    void addEleBuf(_Buffer_t *buffer, unsigned int size) {
        buffers.back().element = new VB_data({(void*)buffer, size});
    }
    template <typename _Buffer_t>
    void addEleBuf(vector<_Buffer_t> const& buffer) {
        addEleBuf(buffer.data(), buffer.size()*sizeof(_Buffer_t));
    }
private:
    unsigned short readNum(string &str) {
        unsigned short num = 0, i;
        for(i=0; i<str.size(); i++) {
            if(isdigit(str[i])) { num = num*10 + str[i]-'0'; }
            else break;
        }
        str = str.substr(i);
        return num;
    }
    void _addAttrib(unsigned short size, GLenum type, unsigned short divisor, unsigned short buffId) {
        buffers.back().buffs[buffId].attribs.push_back({size, type, divisor});
    }
public:
    template <typename ..._Args>
    void addAttrib(string str, _Args... args) {
        string _str_cpy = str;
        unsigned short num = readNum(str);
        if(!num) { num = 1; }
        GLenum type; unsigned short size = 0;
        if(str.substr(0,3) == "vec" || str.substr(0,3) == "col") {
            size = readNum(str = str.substr(3));
        } else if(str.substr(0,3) == "mat") {
            size = readNum(str = str.substr(3));
            unsigned short size2 = size;
            if(str[0] == 'x') { size2 = readNum(str = str.substr(1)); }
            num *= size2;
        } else {
            size = 1;
        }
        unsigned short typelen = 0;
        if(str.substr(0,4) == "bool"  ) { type = GL_BOOL        ; typelen = 4; } else 
        if(str.substr(0,3) == "int"   ) { type = GL_INT         ; typelen = 3; } else 
        if(str.substr(0,4) == "uint"  ) { type = GL_UNSIGNED_INT; typelen = 4; } else 
        if(str.substr(0,5) == "float" ) { type = GL_FLOAT       ; typelen = 5; } else 
        if(str.substr(0,6) == "double") { type = GL_DOUBLE      ; typelen = 6; } else 
        switch(str[0]) {
            case 'b': type = GL_BOOL        ; typelen = 1; break;
            case 'i': type = GL_INT         ; typelen = 1; break;
            case 'u': type = GL_UNSIGNED_INT; typelen = 1; break;
            case 'f': type = GL_FLOAT       ; typelen = 1; break;
            case 'd': type = GL_DOUBLE      ; typelen = 1; break;
            default: std::cerr << "VertexArray::ERROR:: Unknown type: " << str << std::endl; exit(-1);
        }
        str = str.substr(typelen);
        unsigned short divisor = readNum(str);
        if(str.size()) { std::cerr << "VertexArray::ERROR:: Invalid attribute string: " << _str_cpy.c_str() << std::endl; exit(-1); }
        for(int i=0; i<num; i++) {
            _addAttrib(size, type, divisor, _buffId++);
        }
        addAttrib(args...);
    }
    void addAttrib() { }
    void finish() { bind();
        buffers.pop_back();
        unsigned int attribIndex = 0;
        for(auto const&buffer : buffers) {
            VertexBuffer VBO; VBOs.push_back(VBO);
            unsigned int size = 0, i=0;
            GLintptr buffOffsets[buffer.buffs.size()];
            for(auto const&buff : buffer.buffs) {
                buffOffsets[i] = size;
                size += buff.size;
                i++;
            }
            if(!size) { std::cerr << "Error: No buffer data" << std::endl; exit(-1); }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            if(buffer.element) {
                VertexBuffer EBO; VBOs.push_back(EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.element->size, buffer.element->data, GL_STATIC_DRAW);
            }
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
            i = 0;
            for(auto const&buff : buffer.buffs) {
                glBufferSubData(GL_ARRAY_BUFFER, buffOffsets[i], buff.size, buff.data);
                i++;
                unsigned int attLength = 0, j = 0;
                GLintptr attOffsets[buff.attribs.size()];
                for(Attribute const&attrib : buff.attribs) {
                    attLength += attrib.size * size_of(attrib.type);
                    attOffsets[i] = attLength;
                    j++;
                } j = 0;
                for(Attribute const&attrib : buff.attribs) {
                    glEnableVertexAttribArray(attribIndex);
                    glVertexAttribPointer(attribIndex, attrib.size, attrib.type, GL_FALSE, attLength, (void*)(attOffsets[j]+buffOffsets[i]));
                    glVertexAttribDivisor(attribIndex, attrib.divisor);
                    attribIndex++;
                    j++;
                }
            }
        }
        unbind();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        for(Buffer &buffer : buffers) {
            if(buffer.element) { delete buffer.element; }
        }
        buffers.clear();
    }
};
VertexArray *VertexArray::_nowVAO = nullptr;

#endif // _VERTEX_ARRAY_H_