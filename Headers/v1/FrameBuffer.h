#ifndef _FRAME_BUFFER_H
#define _FRAME_BUFFER_H

#include <GL.h>
#include <Shader.h>
#include <string>

class FrameBuffer {
    using string = std::string;
public:
    static constexpr glm::real datas[] = {
         1, 1,   1, 1,
        -1, 1,   0, 1,
         1,-1,   1, 0,
        -1,-1,   0, 0,
    }; static GLuint VAO, VBO;
    Shader shader;
    GLuint FBO, RBO;
    GLuint texture[1];
    bool __first = true;

    FrameBuffer() : FrameBuffer(1,1) {}
    FrameBuffer(string program) : FrameBuffer(1,1, program) {}
    FrameBuffer(GLint width, GLint height, 
        string program = "vec4 getColor(vec4 color) { return color; }") : shader(
        "#version 460 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 texCoord;\n"
        "void main() {\n"
            "gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
            "texCoord = aTexCoord;\n"
        "}\n",(
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texCoord;\n"
        "uniform sampler2D screenTexture;\n"
        "uniform vec2 screenSize, pixelSize;\n"
            + program +
        "void main() {\n"
            "FragColor = getColor(texture(screenTexture, texCoord));\n"
        "}\n").c_str()
    ) {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            //glGenTextures(1, texture); glGenRenderbuffers(1, &RBO);
            resize(width, height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture[0], 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::error << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n"; exit(-1);
            }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void use() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }
    void unuse() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void drawTo(GLuint framebuffer) const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        shader.use();
            glDisable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        shader.unuse();
    } void draw() const { drawTo(0); }
    void resize(GLint width, GLint height, bool changeBuffersSize=true) {
        if(changeBuffersSize) {
            resizeBuffers(width, height);
        }
        shader.use();
            shader.set("screenTexture", 0);
            shader.set("screenSize", glm::vec2(width, height));
            shader.set("pixelSize", glm::vec2(1.0f/width, 1.0f/height));
        shader.unuse();
    }
    void resizeBuffers(GLint width, GLint height) {
        if(!__first)
            glDeleteTextures(1, texture);
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if(!__first)
            glDeleteRenderbuffers(1, &RBO);
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        __first = false;
    }
    operator const GLuint() const { return FBO; }
    operator       GLuint()       { return FBO; }

    static void clear() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    static void use(GLuint framebuffer) { glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); }
    static void init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(datas), datas, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(glm::real), (void*)(0));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(glm::real), (void*)(2*sizeof(glm::real)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif // _FRAME_BUFFER_H