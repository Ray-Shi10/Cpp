#include <vector>
#include <GL.h>
#include <Camera.h>
#include <Shader.h>
#include <Window.h>
#include <Function.h>
#include <Random.h>
//#include <VertexArray.h>

int main() { init(4, 6); Shader::logMaxAttribute();
    Window window("OpenGL 3D");
    glfwSetWindowPos(window, 1500, 1000);
    glm::real aspect = 1;
    window.onSize = [&](GLFWwindow*, int width, int height) {
        window.getFramebufferSizeEvent(width, height, false);
        int size = std::max<int>(width/aspect, height);
        glViewport(width/2-size*aspect/2, height/2-size/2, size*aspect, size);
    }; glfwSetWindowSize(window, 800*aspect, 800);
    window.onKey = [&](GLFWwindow*, int,int,int,int) { glfwSetWindowShouldClose(window, true); };

//#define _COORD

    Shader shader1(
        "#version 460 core\n"
        "layout (location = 0) in vec2  aOffset;\n"
        "layout (location = 1) in vec3  aColor;\n"
#ifdef _COORD
        "layout (location = 2) in float aPosX ;\n"
        "layout (location = 3) in float aPosY ;\n"
#else
        "layout (location = 2) in vec2  aPos  ;\n"
#endif
        "out vec3 fragColor;\n"
        "void main() {\n"
#ifdef _COORD
            "gl_Position = vec4(aPosX + aOffset.x, aPosY + aOffset.x, 0.0f, 1.0f);\n"
#else
            "gl_Position = vec4(aPos + aOffset, 0.0f, 1.0f);\n"
#endif
            "fragColor = aColor;\n"
        "}\n",
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fragColor;\n"
        "void main() {\n"
        "	 FragColor = vec4(fragColor, 1.0f);\n"
        "}\n"
    ), shaderSCR(
        "#version 460 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 fragCoord;\n"
        "void main() {\n"
            "gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
            "fragCoord = aTexCoord;\n"
        "}\n",
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec2 fragCoord;\n"
        "uniform sampler2D screenTexture;\n"
        "void main() {\n"
            "FragColor = texture(screenTexture, fragCoord);\n"
        "}\n"
    );

    constexpr int size = 10, amount = size * size;
    //shader1.use(); shader1.set("size", size); shader1.unuse();
#ifdef _COORD
    //*
    glm::real positions[size];
    for(int x = 0; x < size; x++) {
        positions[x] = x*2.0f/size - 1.0f + 1.0f/size;
    }/*/
    glm::real positions[size*(size+1)];
    for(int x = 0; x < size; x++) {
        for(int y = 0; y <= size; y++) {
            positions[y*size+x] = x*2.0f/size - 1.0f + 1.0f/size;
        }
    }//*/
#else
    glm::vec2 positions[size][size];
    for(int x = 0; x < size; x++) {
        for(int y = 0; y < size; y++) {
            positions[x][y] = glm::vec2(x, y) * 2.0f / float(size) - 1.0f + 1.0f/size;
        }
    }
#endif
    glm::vec2 quadPositions[] = {
        glm::vec2( 0.5f,  0.5f) / glm::real(size),
        glm::vec2( 0.5f, -0.5f) / glm::real(size),
        glm::vec2(-0.5f,  0.5f) / glm::real(size),
        glm::vec2(-0.5f, -0.5f) / glm::real(size),
    };
    glm::vec3 quadColors[] = {
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
    };
    
    GLuint VAO[1]; glGenVertexArrays(1, VAO);
    GLuint VBO[3]; glGenBuffers     (3, VBO);
    glBindVertexArray(VAO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
#ifdef _COORD
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(glm::real), /*
                (void*const)(size*sizeof(glm::real)) /*/ 0 //*/
            );
            glVertexAttribDivisor(2, 1);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(glm::real), 0);
            glVertexAttribDivisor(3, size);
#else
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
            glVertexAttribDivisor(2, 1);
#endif
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
            glBufferData   (GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_STATIC_DRAW);
            ;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glBufferData   (GL_ARRAY_BUFFER, sizeof(quadColors), quadColors, GL_STATIC_DRAW);
            ;
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint scrVAO; glGenVertexArrays(1, &scrVAO);
    GLuint scrVBO; glGenBuffers     (1, &scrVBO);
    constexpr glm::real scrDatas[] = {
         1, 1, 1, 1,
        -1, 1, 0, 1,
         1,-1, 1, 0,
        -1,-1, 0, 0,
    };
    glBindVertexArray(scrVAO);
        glBindBuffer(GL_ARRAY_BUFFER, scrVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(scrDatas), scrDatas, GL_STATIC_DRAW);
            ;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(glm::real), 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(glm::real), (void*)(2*sizeof(glm::real)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint FBO[1]; glGenFramebuffers(1, FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
        GLuint texture[1]; glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.window.width, window.window.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture[0], 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n"; exit(-1);
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaderSCR.use(); shaderSCR.set("screenTexture", 0); shaderSCR.unuse();

    while (!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
        window.clear(glm::vec4(glm::vec3(0.1f), 1.0f), 0);
        shader1.use();
            glBindVertexArray(VAO[0]);
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount);
            glBindVertexArray(0);
        shader1.unuse();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        shaderSCR.use();
            glDisable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glBindVertexArray(scrVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        shaderSCR.unuse();

        glfwSwapBuffers(window); glfwPollEvents(); Sleep(15);
    }

    glfwTerminate();
    return 0;
}