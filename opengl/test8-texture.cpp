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
                  0.01f, 1000.0f, 1.0f, 0.005f, 0.0f);
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
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos, 1.0f);\n"
        "}\n",//*
        "#version 330 core\n"
        "layout (points) in;\n"
        "layout (triangle_strip, max_vertices = 14) out;\n"
        "out vec3 fragColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "uniform sampler2D texture0;\n"
        "void main() {\n"
        "    //if(pos.z > 1.0f) return;\n"
        "    mat4 pvm = projection * view * model;\n"
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f,  1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,1.0f)); EmitVertex();\n" // 0
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f,  1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,1.0f)); EmitVertex();\n" // 1
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f, -1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,0.0f)); EmitVertex();\n" // 2
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f, -1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,0.0f)); EmitVertex();\n" // 3
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f, -1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,0.0f)); EmitVertex();\n" // 7
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f,  1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,1.0f)); EmitVertex();\n" // 1
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f,  1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,1.0f)); EmitVertex();\n" // 5
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f,  1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,1.0f)); EmitVertex();\n" // 0
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f,  1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,1.0f)); EmitVertex();\n" // 4
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f, -1.0f,  1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,0.0f)); EmitVertex();\n" // 2
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f, -1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,1.0f)); EmitVertex();\n" // 6
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f, -1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,0.0f)); EmitVertex();\n" // 7
        "    gl_Position = gl_in[0].gl_Position + vec4( 1.0f,  1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(1.0f,1.0f)); EmitVertex();\n" // 4
        "    gl_Position = gl_in[0].gl_Position + vec4(-1.0f,  1.0f, -1.0f, 0.0f) / 2;\n"
        "    gl_Position = pvm * gl_Position;\n"
        "    fragColor = texture(texture0, vec2(0.0f,1.0f)); EmitVertex();\n" // 5
        "    EndPrimitive();\n"
        "}\n",//*/
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fragColor;\n"
        "void main() {\n"
        "	 FragColor = vec4(fragColor, 1.0f);\n"
        "}\n"
    );

    constexpr glm::ivec3 worldCoordMax(10, 2, 10) , worldCoordMin(-worldCoordMax),
              worldSize = worldCoordMax - worldCoordMin + glm::ivec3(1);
    constexpr unsigned int cubeamount = worldSize.x*worldSize.y*worldSize.z;
    glm::vec3 positions[cubeamount];
    for(int x = 0; x < worldSize.x; x++) {
        for(int y = 0; y < worldSize.y; y++) {
            for(int z = 0; z < worldSize.z; z++) {
                positions[x*worldSize.y*worldSize.z + y*worldSize.z + z] = glm::vec3(x, y, z) + glm::vec3(worldCoordMin);
            }
        }
    }/*
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
    };//*/
    
    unsigned int VAO; glGenVertexArrays(1, &VAO);
    unsigned int VBO; glGenBuffers(1, &VBO);
    //unsigned int EBO; glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, cubeamount*3*sizeof(float), &positions[0][0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
            //glVertexAttribDivisor(0, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    /*
    
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);//*/

    unsigned int texture0; glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width = 16, height = 16; unsigned char data[width*height*4];
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            data[(x + y*width)*4 + 0] = 255*(x+y)%256;
            data[(x + y*width)*4 + 1] = 255*(x+y)%256;
            data[(x + y*width)*4 + 2] = 255*(x+y)%256;
            data[(x + y*width)*4 + 3] = 255;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    //glTexBuffer(GL_TEXTURE_2D, GL_RGB, texture);

    shader1.use();
    shader1.set("model", glm::mat4(1.0f));
    shader1.set("texture0", 0);
    shader1.unuse();

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);

        shader1.use();//*
        shader1.set("projection", camera.getProjectionMatrix());/*/shader1.set("projection", glm::mat4(1.0f));//*/
        shader1.set("view", camera.getViewMatrix());
        shader1.unuse();

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, cubeamount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(15);
    }

    glfwTerminate();
    return 0;
}