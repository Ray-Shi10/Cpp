#ifndef _TEXTURE_H_
#define _TEXTURE_H_

class Texture {
private:
public:
    static Texture *bound;
    using TextureID = GLuint;
    TextureID id;
    Texture() {
        glGenTextures(1, &id);
    }
    ~Texture() {
        glDeleteTextures(1, &id);
    }
    void bind() {
        glBindTexture(GL_TEXTURE_2D, id);
    }
    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

#endif // _TEXTURE_H_