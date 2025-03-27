#include <vector>
#include <GL.h>
#include <Camera.h>
#include <Shader.h>
#include <Window.h>
#include <Function.h>
#include <Random.h>

int main() { initGLFW(4, 6);
    Window window(1,1, "OpenGL 3D");
    glfwSetWindowPos(window, 1800, 900);
    const float aspect = 1;
    window.onSize = [&](GLFWwindow*, int width, int height) {
        window.getFramebufferSizeEvent(width, height, false);
        int size = std::max<int>(width/aspect, height);
        glViewport(width/2-size*aspect/2, height/2-size/2, size*aspect, size);
    }; glfwSetWindowSize(window, 1000*aspect, 1000);
    window.onKey = [](GLFWwindow* window,int,int,int,int) { glfwSetWindowShouldClose(window, true); };

    Shader shader1(
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aOffset;\n"
        "out vec3 fColor;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos*0.05 + aOffset, 0.0f, 1.0f);\n"
        "	 fColor = aColor;\n"
        "}\n",
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fColor;\n"
        "void main() {\n"
        "	 FragColor = vec4(fColor,1.0f);\n"
        "}\n"
    );

    const unsigned int size = 10;
    glm::vec2 positions[size*size];
    for(int x=0; x<size; x++) for(int y=0; y<size; y++) {
        glm::vec2 pos(x*2.0f/size, y*2.0f/size);
        positions[x*size+y] = pos-1.0f+1.0f/size;
    }
    float quadVertices[] = {
        //positions  //colors
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  //0 left  top   
        -1.0f,-1.0f, 0.0f, 1.0f, 0.0f,  //1 left  bottom
         1.0f,-1.0f, 0.0f, 0.0f, 1.0f,  //2 right bottom
         1.0f, 1.0f, 1.0f, 1.0f, 0.0f   //3 right top   
    }; unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int VBO, VAO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*size*size, &positions[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
            glVertexAttribDivisor(2, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); glClear(GL_COLOR_BUFFER_BIT); 

        shader1.use();

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, size*size);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(15);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}