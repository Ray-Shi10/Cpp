#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

#include <GL.h>
#include <string>
#include <vector>

class VertexArray {
private:
    static VertexArray *_nowVAO;
    typedef unsigned int buffID;
    template <typename _T>
    using vector = std::vector<_T>;
    using string = std::string;
public:
    unsigned int VAO; bool binded = false; int attribIndex = 0;
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
        short size; GLenum type; short divisor;
    };
    struct Buffer {
        vector<Attribute> attribs;
        VB_data *element = nullptr, *buffer = nullptr;
    }; vector<Buffer> buffers;
    VertexArray() : buffers(1) { glGenVertexArrays(1, &VAO); }
    ~VertexArray() { glDeleteVertexArrays(1, &VAO);
        for(buffID VBO : VBOs) { glDeleteBuffers(1, &VBO); }
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
    void addBuffer(_Buffer_t *buffer, unsigned int size) { bind();
        //if(!size) { size = sizeof(buffer); }
        buffers.back().buffer  = new VB_data({buffer, size});
    }
    template <typename _Buffer_t>
    void addEleBuf(_Buffer_t *buffer, unsigned int size) { bind();
        buffers.back().element = new VB_data({buffer, size});
    }
private:
    short readNum(string &str) {
        short num = 0, i;
        for(i=0; i<str.size(); i++) {
            if(isdigit(str[i])) { num = num*10 + str[i]-'0'; }
            else break;
        }
        str = str.substr(i);
        return num;
    }
    void _addAttrib(short size, GLenum type, short divisor) {
        buffers.back().attribs.push_back({size, type, divisor});
    }
public:
    template <typename ..._Args>
    void addAttrib(string str, _Args... args) { bind();
        string _str_cpy = str;
        short num = readNum(str);
        if(!num) { num = 1; }
        GLenum type; short size = 0;
        if(str.substr(0,3) == "vec" || str.substr(0,3) == "col") {
            size = readNum(str = str.substr(3));
        } else if(str.substr(0,3) == "mat") {
            size = readNum(str = str.substr(3));
            short size2 = size;
            if(str[0] == 'x') { size2 = readNum(str = str.substr(1)); }
            num *= size2;
        } else {
            size = 1;
        }
        short typelen = 0;
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
        short divisor = readNum(str);
        if(str.size()) { std::cerr << "VertexArray::ERROR:: Invalid attribute string: " << _str_cpy.c_str() << std::endl; exit(-1); }
        for(int i=0; i<num; i++) {
            _addAttrib(size, type, divisor);
        }
        addAttrib(args...);
    }
    void addAttrib() {
        buffers.push_back(Buffer());
    }
    void finish() { bind();
        buffers.pop_back();
        attribIndex = 0;
        for(Buffer &buffer : buffers) {
            VertexBuffer VBO; VBOs.push_back(VBO);
            if(!buffer.buffer) { std::cerr << "Error: No buffer data" << std::endl; exit(-1); }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            if(buffer.element) {
                VertexBuffer EBO; VBOs.push_back(EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.element->size, buffer.element->data, GL_STATIC_DRAW);
            }
            glBufferData(GL_ARRAY_BUFFER, buffer.buffer->size, buffer.buffer->data, GL_STATIC_DRAW);
            unsigned int attLength = 0, i=0;
            unsigned int attLengths[buffer.attribs.size()];
            for(Attribute &attrib : buffer.attribs) {
                attLength += (attLengths[i++] = attrib.size * size_of(attrib.type));
            } i = 0;
            GLintptr offset = 0;
            for(Attribute &attrib : buffer.attribs) {
                glEnableVertexAttribArray(attribIndex);
                glVertexAttribPointer(attribIndex, attrib.size, attrib.type, GL_FALSE, attLength, (void*)offset);
                glVertexAttribDivisor(attribIndex, attrib.divisor);
                offset += attLengths[i++];
                attribIndex++;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        unbind();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        for(Buffer &buffer : buffers) {
            delete buffer.buffer;
            if(buffer.element) {
                delete buffer.element;
            }
        }
        buffers.clear();
    }
};
VertexArray *VertexArray::_nowVAO = nullptr;

#endif // _VERTEX_ARRAY_H_