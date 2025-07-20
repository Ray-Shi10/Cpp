#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

#include <GL.h>
#include <string>
#include <vector>

class VertexArray {
private:
    static VertexArray *_nowVAO;
    using ushort = unsigned short;
    using BufferID = GLuint;/*
    using size_t = unsigned int;/*/
    using size_t = std::size_t;//*/
    template <typename _T>
    using vector = std::vector<_T>;
    using string = std::string;
    //ushort _buffId = 0;
public:
    vector<BufferID> tmpVBOs;
    struct VBdata { void const* data; size_t size; };
    BufferID VAO; bool binded = false;
     VertexArray() { glGenVertexArrays(1, &VAO); }
    ~VertexArray() { glDeleteVertexArrays(1, &VAO); }
    operator BufferID      &() { return VAO; }
    operator BufferID const&() const { return VAO; }
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
private:
    class Stream {
        static ushort getNum_h(string &str, ushort _default=0) { ushort num = 0, i = 0;
            while(i<str.size()&&std::isdigit(str[i])) { num = num<<3 + num<<1 + str[i++] - '0'; }
            if(!i) num = _default; str = str.substr(i); return num;
        }
        void const* _readPointer = nullptr;
        size_t _readPtrSize = 0;
    public:
        VertexArray *VAO;
        BufferID EBO=0; GLenum EBOtype=0; size_t maxIndex=0;
        struct Attributes {
            size_t baseOffset, stride;
            struct Attribute { size_t size, divisor; GLenum type; };
            void addAttribute(size_t size, ushort divisor, GLenum type = GL_BYTE) {
                attributes.push_back({size, type, divisor}); stride += size*size_of(type);
            } vector<Attribute> attributes;
        }; vector<Attributes> attributes;
        vector<VBdata> VBs;
        Stream(VertexArray *VAO) : VAO(VAO) {}
        ~Stream() { size_t offset, size; ushort attribIndex = 0;
            offset = 0, size = 0;
            for(auto &VB : VBs) { size += VB.size; }
            BufferID VBO; glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
            for(auto &VB : VBs) {   
                glBufferSubData(GL_ARRAY_BUFFER, offset, VB.size, VB.data);
                offset += VB.size;
            }
            for(auto &attribs : attributes) {
                size_t offset = attribs.baseOffset;
                for(auto &attrib : attribs.attributes) {
                    glEnableVertexAttribArray(attribIndex);
                    glVertexAttribPointer(attribIndex, attrib.size, attrib.type, GL_FALSE, attribs.stride, (void*)offset);
                    glVertexAttribDivisor(attribIndex, attrib.divisor);
                    offset += attrib.size*size_of(attrib.type); attribIndex++;
                }
            }
            VertexArray::clear();
            if(EBO) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
        }
        template <typename T>
        Stream &operator<<(vector<T>const&data) {
            if(_readPointer) {
                std::error << "ERROR: No size specified for the data\n";
            }
            VBs.push_back({data.data(), data.size()*sizeof(T)});
            return *this;
        }/*
        template <typename T>
        Stream &operator<<(T *data) {
            if(_readPointer) {
                throw "ERROR: No size specified for the data";
            }
            _readPointer = data;
            _readPtrSize = sizeof(T);
            return *this;
        }*/
        Stream &operator<<(size_t _size_t) {/*
            if(_readPointer) {
                size_t &size = _size_t;
                VBs.push_back({_readPointer, size, _readPtrSize*size});
                _readPointer = nullptr; _readPtrSize = 0;
            } else*/
            {
                ;
            }
            return *this;
        }
        Stream &operator<<(string attribs) {/*
            if(_readPointer) {
                throw "ERROR: No size specified for the data";
            }*/
            ushort amount = getNum_h(attribs, 1), size = 1, divisor = 0; GLenum typeEnum = 0;/*
            if(attribs[0] == '+') { attribs = attribs.substr(1);
                size = getNum_h(attribs, maxIndex); typeEnum = 1;
            } else {*/
                string attTypeHead = attribs.substr(0, 3); attribs = attribs.substr(3);
                if(attTypeHead == "vec") { size = getNum_h(attribs, 3); } else 
                if(attTypeHead == "col") { size = getNum_h(attribs, 3); } else
                if(attTypeHead == "mat") { size = getNum_h(attribs, 4); ushort size2 = size;
                    amount *= (attribs[0]=='x' ? getNum_h(attribs=attribs.substr(1), 4) : size);
                }
            /*}*/ ushort typeSize = 0;
            switch(attribs[0]) {
                case 'b':   typeEnum = GL_BOOL            ; break;
                case 'B':   typeEnum = GL_BYTE            ; break;
                case 's':   typeEnum = GL_SHORT           ; break;
                case 'S':   typeEnum = GL_UNSIGNED_SHORT  ; break;
                case 'f':   typeEnum = GL_FLOAT           ; break;
                case 'd':   typeEnum = GL_DOUBLE          ; break;
                case 'i':   typeEnum = GL_INT             ; break;
                case 'u':   typeEnum = GL_UNSIGNED_INT    ; break;
                default :/*if(!typeEnum)*/{typeEnum = GL_FLOAT; break;}
            } //typeSize /*+*/= size_of(typeEnum/*, attributes.back().stride*/);
            if(attribs.size()) { attribs = attribs.substr(1);
            if(attribs[0] == '-') { attribs = attribs.substr(1);
                divisor = getNum_h(attribs, 1);
            } }
            attributes.back().addAttribute(size, divisor, typeEnum);
            return *this;
        }
    };
public:
    template <typename ...Args>
    Stream operator()(size_t maxIndex, Args ...args) { bind();
        Stream stream; stream.maxIndex = maxIndex;
        _addEleBuffs(stream, args...);
        return stream;
    }
private:
    template <typename T>
    void _addEleBuff(Stream &stream, T *data, size_t size, GLenum type) {
        BufferID EBO; glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size*sizeof(T), data, GL_STATIC_DRAW);
        stream.EBO = EBO; stream.EBOtype = type;
    }
    template <typename T>
    void _addEleBuffs(Stream &stream, vector<T> indices, GLenum type = GLenumType<T>::value) {
        _addEleBuff(stream, indices.data(), indices.size(), type);
    }
    template <typename T>
    void _addEleBuffs(Stream &stream, T *data, size_t size, GLenum type = GLenumType<T>::value) {
        _addEleBuff(stream, data, size, type);
    }
    void _addEleBuffs(Stream &stream) {}
};
VertexArray *VertexArray::_nowVAO = nullptr;

#endif // _VERTEX_ARRAY_H_