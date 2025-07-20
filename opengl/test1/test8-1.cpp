//8
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
    Camera camera(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(), glm::vec3(), 
                  0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
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
        "//layout (location = 3) in float aSize;\n"
        "out vs2gs { vec3 color; } attrib;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "    vec3 pos = aCubePos + aOffset;//*aSize;\n"
        "    gl_Position = projection * view * model * vec4(pos, 1.0f);\n"
        "    attrib.color = aColor;\n"
        "}\n",//*
        "#version 330 core\n"
        "layout (triangles) in;\n"
        "layout (triangle_strip, max_vertices = 24) out;\n"
        "in vs2gs { vec3 color; } attribs[];\n"
        "out vec3 fragColor;\n"
        "void main() {\n"
        "    for(int i = 0; i < 14; i++) {\n"
        "        gl_Position = gl_in[i].gl_Position;\n"
        "        fragColor = attribs[i].color;\n"
        "        EmitVertex();\n"
        "    }\n"
        "    EndPrimitive();\n"
        "}\n",//*/
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fragColor;\n"
        "void main() {\n"
        "	 FragColor = vec4(fragColor, 1.0f);\n"
        "    if(gl_FragCoord.x < 10) FragColor = vec4(0,0,0,1);\n"
        "    FragColor.r = (gl_FragCoord.z+1)/2;\n"
        "}\n"
    );

    const glm::ivec3 worldCoordMax(10, 2, 10) , worldCoordMin(-worldCoordMax),
              worldSize = worldCoordMax - worldCoordMin + glm::ivec3(1);
    glm::vec3 positions[worldSize.x*worldSize.y*worldSize.z];
    const unsigned int cubeamount = worldSize.x*worldSize.y*worldSize.z;
    for(int x = 0; x < worldSize.x; x++) {
        for(int y = 0; y < worldSize.y; y++) {
            for(int z = 0; z < worldSize.z; z++) {
                positions[x*worldSize.y*worldSize.z + y*worldSize.z + z] = glm::vec3(x, y, z) + glm::vec3(worldCoordMin);
            }
        }
    }

    const glm::vec3 cubeVertices[] = {
        glm::vec3( 1.0f, 1.0f, 1.0f) / 2.0f,
        glm::vec3(-1.0f, 1.0f, 1.0f) / 2.0f,
        glm::vec3( 1.0f,-1.0f, 1.0f) / 2.0f,
        glm::vec3(-1.0f,-1.0f, 1.0f) / 2.0f,
        glm::vec3( 1.0f, 1.0f,-1.0f) / 2.0f,
        glm::vec3(-1.0f, 1.0f,-1.0f) / 2.0f,
        glm::vec3( 1.0f,-1.0f,-1.0f) / 2.0f,
        glm::vec3(-1.0f,-1.0f,-1.0f) / 2.0f,
    }; const unsigned short indices[] = { 0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5 };

    const glm::vec3 cubeColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
    };
    
    unsigned int VBO, VAO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);
    glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices) + sizeof(cubeColors), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVertices), cubeVertices);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVertices), sizeof(cubeColors), cubeColors);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(sizeof(cubeVertices)));
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions[0][0], GL_STATIC_DRAW);
            //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions[0][0]);
            //glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(sizes), sizes);

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
            glVertexAttribDivisor(2, 1);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1*sizeof(float), (void*)sizeof(positions));
            glVertexAttribDivisor(3, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLbitfield flags = GL_MAP_WRITE_BIT
    | GL_MAP_PERSISTENT_BIT //在被映射状态下不同步
    | GL_MAP_COHERENT_BIT; //数据对GPU立即可见
    //为Buffer分配数据，取代之前的glBufferData
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(positions), positions, flags);
    //映射一次即可，保存该指针后用于渲染时使用
    GLvoid* dataPtr = glMapBuffer(GL_ARRAY_BUFFER, flags);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
        glDrawElementsInstanced(GL_TRIANGLE_STRIP, sizeof(indices)/sizeof(unsigned short), GL_UNSIGNED_SHORT, 0, cubeamount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(15);
    }
    
    glfwTerminate();
    return 0;
}