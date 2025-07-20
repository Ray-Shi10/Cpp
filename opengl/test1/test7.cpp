#include <vector>
#include <GL.h>
#include <Camera.h>
#include <Shader.h>
#include <Window.h>
#include <Function.h>
#include <Random.h>
#include <VertexArray.h>

int main() { initGLFW(4, 6);
    Window window("OpenGL 3D");
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), 
                  0.1f, 100.0f, 3.0f, 0.005f, 0.0f);
    glfwSetWindowPos(window, 1500, 900);
    float aspect = glm::sqrt2;
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

    std::vector<glm::vec3> positions({
        glm::vec3( 0.0f,  0.0f,  10.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, - 2.5f),
        glm::vec3(-3.8f, -2.0f,  12.3f),
        glm::vec3( 2.4f, -0.4f, - 3.5f),
        glm::vec3(-1.7f,  3.0f,   7.5f),
        glm::vec3( 1.3f, -2.0f, - 2.5f),
        glm::vec3( 1.5f,  2.0f, - 2.5f),
        glm::vec3( 1.5f,  0.2f, - 1.5f),
        glm::vec3(-1.3f,  1.0f,   1.5f),
    }); unsigned short cubeamount = positions.size();
    std::vector<float> sizes(cubeamount);
    for(int i=0; i<cubeamount; i++) { sizes[i] = Rand::random(0.2f, 0.4f); }

    std::vector<glm::vec3> cubeVertices({
        glm::vec3( 1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3( 1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3( 1.0f, 1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),
        glm::vec3( 1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),
    }); std::vector<unsigned short> indices({0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5});

    std::vector<glm::vec3> cubeColors({
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
    });
    VertexArray VAO;
    //*
    VAO(8, indices) << cubeVertices << "vec3" << cubeColors << "col3";
    VAO(cubeamount) << positions << "vec3-1" << sizes << "float-1";//*/

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
        glDrawElementsInstanced(GL_TRIANGLE_STRIP, sizeof(indices)/sizeof(unsigned short), GL_UNSIGNED_INT, 0, cubeamount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(15);
    }

    glfwTerminate();
    return 0;
}