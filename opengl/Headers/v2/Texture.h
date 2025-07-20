#ifndef _TEXTURE_H_
#define _TEXTURE_H_

class Texture {
private:
public:
    static Texture *bound;
    using TextureID = GLuint;
    TextureID id;
    bool binded = 0;
     Texture() { glGenTextures   (1, &id); }
    ~Texture() { glDeleteTextures(1, &id); }
    void bind() {
        if(bound != this) {
            if(bound)
                bound -> binded = false;
            binded = true;
            glBindTexture(GL_TEXTURE_2D, id);
            bound = this;
        }
    }
    void unbind() {
        if(bound == this) {
            bound = nullptr;
            binded = false;
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    static void clear() {
        if(bound) bound -> unbind();
    }
};

#endif // _TEXTURE_H_