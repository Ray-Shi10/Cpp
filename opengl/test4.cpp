#include <GL.h>
#include <Camera.h>
#include <Shader.h>
#include <Window.h>
#include <Function.h>
#include <vector>

int main() { initGLFW(4, 6);
    Window window(1,1, "OpenGL 3D");
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), 
                  0.1f, 100.0f, 0.1f, 0.005f, 0.0f);
    glfwSetWindowPos(window, 1500, 1000);
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
    glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
        printf("Cursor %-7s window\n", entered ? "entered" : "left");
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        printf("Mouse %-6s %8s", (button == GLFW_MOUSE_BUTTON_LEFT ? "left" : (button == GLFW_MOUSE_BUTTON_RIGHT ? "right" : "middle")), (action == GLFW_PRESS ? "pressed" : "released"));
    });

    window.setCursorMode(GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader shader1(
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aColor;\n"
        "out vec2 Color;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
        "	 Color = aColor;\n"
        "}\n",
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 Color;\n"
        "void main() {\n"
        "	 FragColor = vec4(Color,0.0f,1.0f);\n"
        "}\n"
    );

    float vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, //0
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, //1
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, //2
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, //3
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, //4
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, //5
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, //6
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f  //7
    };
    unsigned int indices[] = {
        0, 1, 2,    1, 2, 3,
        4, 5, 6,    5, 6, 7,
        0, 1, 4,    1, 4, 5,
        2, 3, 6,    3, 6, 7,
        0, 2, 4,    2, 4, 6,
        1, 3, 5,    3, 5, 7
    };
    std::vector<glm::vec3> cubePositions = {
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
    }; const short cubeamount = cubePositions.size();
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    // set argument: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // set argument: color
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
            if(window.keyPress(GLFW_KEY_W)) { move += glm::vec3( 0.0f, 0.0f,-1.0f); }
            if(window.keyPress(GLFW_KEY_S)) { move += glm::vec3( 0.0f, 0.0f, 1.0f); }
            if(window.keyPress(GLFW_KEY_A)) { move += glm::vec3(-1.0f, 0.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_D)) { move += glm::vec3( 1.0f, 0.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_SPACE)) { move += glm::vec3( 0.0f, 1.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_LEFT_SHIFT)) { move += glm::vec3( 0.0f,-1.0f, 0.0f); }
            if(window.keyPress(GLFW_KEY_R)) {
                camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
                camera.dir = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            camera.move(move);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        shader1.use();

        //*
        shader1.set("projection", camera.getProjectionMatrix());/*/
        shader1.set("projection", glm::mat4(1.0f));//*/
        //shader1.set("projection", glm::perspective(glm::radians(45.0f), (float)window.window.width / (float)window.window.height, 0.01f, 1000.0f));

        shader1.set("view", camera.getViewMatrix());

        glBindVertexArray(VAO);//*
        for (unsigned int i = 0; i < cubeamount; i++) {/*/
        int i=0; {//*/
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader1.set("model", model);/*
            glDrawArrays(GL_TRIANGLES, 0, 36);/*/
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);//*/
        }
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