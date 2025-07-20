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
                  0.01f, 1000.0f, 5.0f, 0.005f, 0.0f);
    glfwSetWindowPos(window, 1500, 1000);
    float aspect = glm::sqrt2;
    window.onSize = [&](GLFWwindow*, int width, int height) {
        window.getFramebufferSizeEvent(width, height, false);
        int size = std::max<int>(width/aspect, height);
        camera.view = glm::vec3(size*aspect, size, size);
        glViewport(width/2-size*aspect/2, height/2-size/2, size*aspect, size);
    }; glfwSetWindowSize(window, 800*aspect, 800);
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
        "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in uint blockType;\n"
        "out vs2gs { uint blockType; } attrib;\n"
        "void main() {\n"
            "gl_Position = vec4(aPos, 1.0f);\n"
            "attrib.blockType = blockType;\n"
        "}\n",//*
        "#version 460 core\n"
        "layout (points) in;\n"
        "in vs2gs { uint blockType; } attribs[];\n"
        "layout (triangle_strip, max_vertices = 14) out;\n"
        "out vec3 fragColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 camTrans;\n"
        "void addPoint(vec3 offset, vec3 color) {\n"
            "gl_Position = camTrans * ( gl_in[0].gl_Position + vec4(offset/2, 0.0f) );\n"
            "fragColor = color; EmitVertex();\n"
        "}\n"
        "void main() {\n"
            "//if(pos.z > 1.0f) return;\n"
            "if(attribs[0].blockType == 0U) return;\n"
             // 0 1 2 3 7 1 5 0 4 2 6 7 4 5
            "addPoint(vec3( 1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f));\n" // 0
            "addPoint(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));\n" // 1
            "addPoint(vec3( 1.0f,-1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f));\n" // 2
            "addPoint(vec3(-1.0f,-1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));\n" // 3
            "addPoint(vec3(-1.0f,-1.0f,-1.0f), vec3(0.0f, 0.0f, 0.0f));\n" // 7
            "addPoint(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));\n" // 1
            "addPoint(vec3(-1.0f, 1.0f,-1.0f), vec3(0.0f, 1.0f, 0.0f));\n" // 5
            "addPoint(vec3( 1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f));\n" // 0
            "addPoint(vec3( 1.0f, 1.0f,-1.0f), vec3(1.0f, 1.0f, 0.0f));\n" // 4
            "addPoint(vec3( 1.0f,-1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f));\n" // 2
            "addPoint(vec3( 1.0f,-1.0f,-1.0f), vec3(1.0f, 0.0f, 1.0f));\n" // 6
            "addPoint(vec3(-1.0f,-1.0f,-1.0f), vec3(0.0f, 0.0f, 0.0f));\n" // 7
            "addPoint(vec3( 1.0f, 1.0f,-1.0f), vec3(1.0f, 1.0f, 0.0f));\n" // 4
            "addPoint(vec3(-1.0f, 1.0f,-1.0f), vec3(0.0f, 1.0f, 0.0f));\n" // 5
            "EndPrimitive();\n"
        "}\n",//*/
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fragColor;\n"
        "void main() {\n"
        "	 FragColor = vec4(fragColor, 1.0f);\n"
        "}\n"
    );

    constexpr glm::ivec3 viewCoordMax(2) , viewCoordMin(-viewCoordMax),
              viewSize = viewCoordMax - viewCoordMin + glm::ivec3(1);
    constexpr unsigned int cubeamount = viewSize.x*viewSize.y*viewSize.z;
    glm::vec3 positions[viewSize.x][viewSize.y][viewSize.z];
    for(int x = 0; x < viewSize.x; x++) {
        for(int y = 0; y < viewSize.y; y++) {
            for(int z = 0; z < viewSize.z; z++) {
                positions[x][y][z] = glm::vec3(x, y, z) + glm::vec3(viewCoordMin);
            }
        }
    }
    GLenum blockTypes[viewSize.x][viewSize.y][viewSize.z];
    for(int x = 0; x < viewSize.x; x++) {
        for(int y = 0; y < viewSize.y; y++) {
            for(int z = 0; z < viewSize.z; z++) {
                blockTypes[x][y][z] = !(x==0 || y==0 || z==0 || x==viewSize.x-1 || y==viewSize.y-1 || z==viewSize.z-1);
            }
        }
    }
    blockTypes[2][2][2] = 0;
    
    unsigned int VAO ; glGenVertexArrays(1, &VAO );
    unsigned int VBO ; glGenBuffers     (1, &VBO );
    unsigned int VBO2; glGenBuffers     (1, &VBO2);
    //unsigned int EBO; glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
            glBufferData(GL_ARRAY_BUFFER, cubeamount*sizeof(GLenum), blockTypes, GL_STATIC_DRAW)
            ;
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1*sizeof(float), (void*)(0));
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, cubeamount*3*sizeof(float), &positions[0][0][0], GL_STATIC_DRAW);
            ;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0));
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

        shader1.use();
        shader1.set("camTrans", camera.getMatrix());
        shader1.set("model", glm::mat4(1.0f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, viewSize.y*viewSize.z+viewSize.z+1, viewSize.x*viewSize.y*viewSize.z-2*viewSize.y*viewSize.z-2*viewSize.z-2);
        glBindVertexArray(0);
        shader1.unuse();

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(15);
    }

    glfwTerminate();
    return 0;
}