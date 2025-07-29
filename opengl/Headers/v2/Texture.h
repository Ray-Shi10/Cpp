#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "base.h"

class Texture2D {
private:
  using Texture = Texture2D;
public:
  static GLuint textureAmount;
  static Texture *bound;
  using TextureID = GLuint;
  TextureID id;
  bool binded = 0;
   Texture2D() { glGenTextures   (1, &id); textureAmount++; }
  ~Texture2D() { glDeleteTextures(1, &id); textureAmount--; unbind(); }
  void set(GLenum pname, GLint param) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, pname, param);
  }
  void set(GLenum pname, const GLint *params) {
    bind();
    glTexParameteriv(GL_TEXTURE_2D, pname, params);
  }
  void set(GLenum pname, GLfloat param) {
    bind();
    glTexParameterf(GL_TEXTURE_2D, pname, param);
  }
  void set(GLenum pname, const GLfloat *params) {
    bind();
    glTexParameterfv(GL_TEXTURE_2D, pname, params);
  }
  void set(GLenum wrap_s, GLenum wrap_t,
           GLenum min_filter, GLenum mag_filter) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
  }
  void generateMipmaps() {
    bind();
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  void setImage(GLint level, GLint internalformat, GLsizei width, GLsizei height,
                GLint border, GLenum format, GLenum type, const void *data) {
    bind();
    glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, data);
  }

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
    if(bound) bound -> binded = bound = 0;
  }
};

#endif // _TEXTURE_H_