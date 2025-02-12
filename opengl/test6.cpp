#include <vector>
#include <GL.h>
#include <Camera.h>
#include <Shader.h>
#include <Window.h>
#include <Function.h>
#include <Random.h>

int main() { initGLFW(4, 6);
    Window window("OpenGL 3D");
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), 
                  0.1f, 100.0f, 3.0f, 0.005f, 0.0f);
    glfwSetWindowPos(window, 1500, 900);
    const float aspect = glm::sqrt2;
    window.onSize = [&](GLFWwindow*, int width, int height) {
        window.getFramebufferSizeEvent(width, height, false);
        int size = std::max<int>(width/aspect, height);
        camera.view = glm::vec3(size*aspect, size, size);
        glViewport(width/2-size*aspect/2, height/2-size/2, size*aspect, size);
    }; glfwSetWindowSize(window, 1000*aspect, 1000);
    window.onCursorMove = [&](GLFWwindow*, double xpos, double ypos) {
        window.getCursorMoveEvent(xpos, ypos);
        camera.rotate(glm::vec3(window.mouse.offset.y, window.mouse.offset.x, 0.0f));
    };
    window.onKey = [&](GLFWwindow*, int key, int scancode, int action, int mods) {
        window.getKeyEvent(key, scancode, action, mods);
        if(mods & GLFW_MOD_CONTROL) {
            glfwSetWindowShouldClose(window, true);
        }
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            window.window.active ^= 1;
        }
    };
    window.onFocus = [&](GLFWwindow*, int focused) {
        window.getFocusEvent(focused);
        window.window.active &= focused;
    };

    window.setCursorMode(GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    Shader shader1(
        "#version 330 core\n"
        "layout (location = 0) in vec3 aOffset;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec3 aCubePos;\n"
        "layout (location = 3) in float aSize;\n"
        "out vec3 fColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
            "vec3 pos = aCubePos + aOffset*aSize;\n"
            "gl_Position = projection * view * model * vec4(pos, 1.0f);\n"
            "fColor = aColor;\n"
        "}\n",
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fColor;\n"
        "void main() {\n"
        "	 FragColor = vec4(fColor,1.0f);\n"
        "}\n"
    );

    glm::vec3 positions[] = {
        glm::vec3( 0.0f,  0.0f,  10.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, - 2.5f),
        glm::vec3(-3.8f, -2.0f,  12.3f),
        glm::vec3( 2.4f, -0.4f, - 3.5f),
        glm::vec3(-1.7f,  3.0f,   7.5f),
        glm::vec3( 1.3f, -2.0f, - 2.5f),
        glm::vec3( 1.5f,  2.0f, - 2.5f),
        glm::vec3( 1.5f,  0.2f, - 1.5f),
        glm::vec3(-1.3f,  1.0f,   1.5f)
    }; const short cubeamount = sizeof(positions)/sizeof(glm::vec2);
    float sizes[cubeamount];
    for(int i=0; i<cubeamount; i++) {//*
        sizes[i] = Rand::random(0.2f, 0.4f);/*/sizes[i] = 1.0f;//*/
    }

    float cubeVertices[] = {
        //positions         //colors
         1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,    //0
        -1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,    //1
         1.0f,-1.0f, 1.0f,  1.0f, 0.0f, 1.0f,    //2
        -1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,    //3
         1.0f, 1.0f,-1.0f,  1.0f, 1.0f, 0.0f,    //4
        -1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,    //5
         1.0f,-1.0f,-1.0f,  1.0f, 0.0f, 0.0f,    //6
        -1.0f,-1.0f,-1.0f,  0.0f, 0.0f, 0.0f     //7
    }; unsigned int indices[] = {
        0, 1, 2,   1, 2, 3,    // front
        4, 5, 6,   5, 6, 7,    // back
        0, 1, 4,   1, 4, 5,    // top
        2, 3, 6,   3, 6, 7,    // bottom
        0, 2, 4,   2, 4, 6,    // right
        1, 3, 5,   3, 5, 7     // left
    };
    unsigned int VBO, VAO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);//*
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions)+sizeof(sizes), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions[0][0]);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(sizes), sizes);/*/
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions[0][0], GL_STATIC_DRAW);//*/

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
            glVertexAttribDivisor(2, 1);//*
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1*sizeof(float), (void*)sizeof(positions));
            glVertexAttribDivisor(3, 1);//*/
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        window.newFrame();
        window.applyCursorMode();

        if(!window.window.active) {
            window.mouse.first = true;
            glfwWaitEvents();
            continue;
        }

        {
            glm::vec3 move(0.0f);
            if(window.keyPress(GLFW_KEY_R)) { camera.pos = camera.dir = glm::vec3(0.0f); }
            if(window.keyPress(GLFW_KEY_W)) { move += glm::vec3( 0.0f, 0.0f,-1.0f); }
            if(window.keyPress(GLFW_KEY_S)) { move += glm::vec3( 0.0f, 0.0f, 1.0f); }
            if(window.keyPress(GLFW_KEY_A)) { move += glm::vec3(-1.0f, 0.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_D)) { move += glm::vec3( 1.0f, 0.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_SPACE)) { move += glm::vec3( 0.0f, 1.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_LEFT_SHIFT)) { move += glm::vec3( 0.0f,-1.0f, 0.0f); }
            camera.move(move, window.frame.dt);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        shader1.use();//*
        shader1.set("projection", camera.getProjectionMatrix());/*/shader1.set("projection", glm::mat4(1.0f));//*/
        shader1.set("view", camera.getViewMatrix());
        shader1.set("model", glm::mat4(1.0f));

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, cubeamount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(15);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);
    glfwTerminate();
    return 0;
}