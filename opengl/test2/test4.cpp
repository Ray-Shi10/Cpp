#include <vector>
#include <v2/Shader.h>
#include <v2/Canvas.h>
#include <v2/Window.h>
#include <v2/VertexArray.h>
#include <v2/Camera.h>

int main() {
    using std::vector;
    initGLFW(4,6);
    Window window("Test", glm::uvec2(800, 600), glm::vec2(1200, 800));
    // FrameBuffer FBO(
    //     "vec4 getColor(vec4 color) {\n"
    //         "const vec2 offset = pixelSize;\n"
    //         "const vec2 offsets[9] = vec2[](\n"
    //             "vec2(-offset.x,  offset.y), // 左上\n"
    //             "vec2( 0.0f    ,  offset.y), // 正上\n"
    //             "vec2( offset.x,  offset.y), // 右上\n"
    //             "vec2(-offset.x,  0.0f    ), // 正左\n"
    //             "vec2( 0.0f    ,  0.0f    ), // 正中\n"
    //             "vec2( offset.x,  0.0f    ), // 正右\n"
    //             "vec2(-offset.x, -offset.y), // 左下\n"
    //             "vec2( 0.0f    , -offset.y), // 正下\n"
    //             "vec2( offset.x, -offset.y)  // 右下\n"
    //         ");\n"
    //         "float kernel[9] = float[](\n"
    //             "-1.0f, -1.0f, -1.0f, \n"
    //             "-1.0f,  9.0f, -1.0f, \n"
    //             "-1.0f, -1.0f, -1.0f  \n"
    //         ");\n"
    //         "vec4 sampleTex[9];\n"
    //         "for(int i = 0; i < 9; i++) {\n"
    //             "sampleTex[i] = texture(screenTexture, texCoord + offsets[i]);\n"
    //         "}\n"
    //         "vec4 col = vec4(0.0);\n"
    //         "for(int i = 0; i < 9; i++) {\n"
    //             "col += sampleTex[i] * kernel[i];\n"
    //         "}\n"
    //         "return col;\n"
    //     "}\n"
    // ), FBO2; GLushort type=0;
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800,600,1000),
                  0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
    const Camera startCamera = camera;
    glfwSetWindowPos(window, 1500, 1000);
    // float aspect = glm::sqrt2;
    // window.onSize = [&](GLFWwindow*, int width, int height) {
    //     window.getFramebufferSizeEvent(width, height, false);
    //     int size = std::max<int>(width/aspect, height);
    //     camera.view = glm::vec3(size*aspect, size, size);
    //     glViewport(width/2-size*aspect/2, height/2-size/2, size*aspect, size);
    //     FBO.resize(width, height); FBO2.resize(width, height);
    // }; glfwSetWindowSize(window, 800*aspect, 800);
    // window.onCursorMove = [&](GLFWwindow*, double xpos, double ypos) {
    //     window.getCursorMoveEvent(xpos, ypos);
    //     camera.rotate(glm::vec3(window.mouse.offset.y, window.mouse.offset.x, 0.0f));
    // };
    // window.onKey = [&](GLFWwindow*, int key, int scancode, int action, int mods) {
    //     window.getKeyEvent(key, scancode, action, mods);
    //     if(mods & GLFW_MOD_CONTROL) {
    //         glfwSetWindowShouldClose(window, true);
    //     }
    //     if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    //         window.window.active ^= 1;
    //     }
    // };
    // window.onFocus = [&](GLFWwindow*, int focused) {
    //     window.getFocusEvent(focused);
    //     window.window.active &= focused;
    // };
    // window.setCursorMode(GLFW_CURSOR_DISABLED);
    // glEnable(GL_DEPTH_TEST);
    window.setCursorMode(GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, make_function([&](GLFWwindow*, int key, int scancode, int action, int mods) {
        if(action == GLFW_PRESS) {
            switch(key) {
                case GLFW_KEY_ESCAPE:
                    window.window.active ^= 1;
                    std::cout << "Window active: " << window.window.active << "\n";
                    break;
                case GLFW_KEY_TAB:
                    camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
                    camera.dir = glm::vec3(0.0f, 0.0f, 0.0f);
                    break;
                case GLFW_KEY_LEFT_CONTROL:
                    window.close();
                    break;
                default: break;
            }
        }
    }));
    glfwSetCursorPosCallback(window, make_function([&](GLFWwindow*, double xpos, double ypos) {
        if(window.mouse.entering) {
            window.mouse.last = glm::vec2(xpos, ypos);
            window.mouse.entering = false;
        }
        std::debug << "Mouse pos: " << xpos << ", " << ypos << "\n";
        window.mouse.pos = glm::vec2(xpos, ypos);
        window.mouse.offset = window.mouse.pos - window.mouse.last;
        window.mouse.last = window.mouse.pos;
        if(window.window.active)
            camera.rotate(glm::vec3(window.mouse.offset.y, window.mouse.offset.x, 0.0f));
    }));
    glfwSetWindowFocusCallback(window, make_function([&](GLFWwindow*, int focused) {
        window.window.active = focused;
        //window.mouse.entering = true;
        std::debug << "Window focus: " << focused << "\n";
    }));

    glEnable(GL_DEPTH_TEST);

    ShaderProgram shader1(
        // "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in uint blockType;\n"
        "out vs2gs { uint blockType; } attrib;\n"
        "void main() {\n"
            "gl_Position = vec4(aPos, 1.0f);\n"
            "attrib.blockType = blockType;\n"
        "}\n",
        // "#version 460 core\n"
        "layout (points) in;\n"
        "in vs2gs { uint blockType; } attribs[];\n"
        "layout (triangle_strip, max_vertices = 14) out;\n"
        "out vec3 fragColor;\n"
        //"uniform mat4 model;\n"
        "uniform mat4 camTrans;\n"
        "void addPoint(vec3 offset, vec3 color) {\n"
            "gl_Position = camTrans * ( gl_in[0].gl_Position + vec4(offset/2, 0.0f) );\n"
            "fragColor = color;"
            "EmitVertex();\n"
        "}\n"
        "void main() {\n"
            //"if(pos.z > 1.0f) return;\n"
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
        "}\n",
        // "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec3 fragColor;\n"
        "void main() {\n"
        "  FragColor = vec4(fragColor, 1.0f);\n"
        "}\n"
    );

    constexpr glm::ivec3 viewCoordMax(2) , viewCoordMin(-viewCoordMax),
              viewSize = viewCoordMax - viewCoordMin + glm::ivec3(1);
    constexpr GLuint cubeamount = viewSize.x*viewSize.y*viewSize.z;
    glm::vec3 positions[viewSize.x][viewSize.y][viewSize.z];
    for(int x = 0; x < viewSize.x; x++) {
        for(int y = 0; y < viewSize.y; y++) {
            for(int z = 0; z < viewSize.z; z++) {
                positions[x][y][z] = glm::vec3(x, y, z) + glm::vec3(viewCoordMin);
            }
        }
    }

    GLuint VAO ; glGenVertexArrays(1, &VAO );
    GLuint VBO ; glGenBuffers     (1, &VBO );
    GLuint VBO2; glGenBuffers     (1, &VBO2);
    GLenum blockTypes[viewSize.x][viewSize.y][viewSize.z];
    for(int x = 0; x < viewSize.x; x++) {
        for(int y = 0; y < viewSize.y; y++) {
            for(int z = 0; z < viewSize.z; z++) {
                blockTypes[x][y][z] = !(x==0 || y==0 || z==0 || x==viewSize.x-1 || y==viewSize.y-1 || z==viewSize.z-1);
            }
        }
    }
    //blockTypes[2][2][2] = 0;
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, cubeamount*3*sizeof(float), &positions[0][0][0], GL_STATIC_DRAW);
            ;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0));
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
            glBufferData(GL_ARRAY_BUFFER, cubeamount*sizeof(GLenum), blockTypes, GL_STATIC_DRAW);
            ;
            glEnableVertexAttribArray(1);
            glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 1*sizeof(GLenum), (void*)(0));
            ;
            const GLenum _tmp[1] = {0U};
            int cx = viewSize.x / 2;
            int cy = viewSize.y / 2;
            int cz = viewSize.z / 2;
            size_t index = cx * (viewSize.y * viewSize.z) + cy * viewSize.z + cz;
            size_t offset = index * sizeof(GLenum);
            glBufferSubData(GL_ARRAY_BUFFER, (&blockTypes[2][2][2]-&blockTypes[0][0][0])*sizeof(GLenum), sizeof(GLenum), _tmp);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // for(int x = 0; x < viewSize.x; x++) {
    //     for(int y = 0; y < viewSize.y; y++) {
    //         for(int z = 0; z < viewSize.z; z++) {
    //             blockTypes[x][y][z] = !(x==0 || y==0 || z==0 || x==viewSize.x-1 || y==viewSize.y-1 || z==viewSize.z-1);
    //         }
    //     }
    // } blockTypes[2][2][2] = 0;

    // Example: set the block in the center to 0 (delete it)
    // int cx = viewSize.x / 2;
    // int cy = viewSize.y / 2;
    // int cz = viewSize.z / 2;
    // blockTypes[cx][cy][cz] = 0;
    // // Compute linear index for the 3D array
    // size_t index = cx * (viewSize.y * viewSize.z) + cy * viewSize.z + cz;
    // size_t offset = index * sizeof(GLenum);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLenum), &blockTypes[cx][cy][cz]);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    while(!window.shouldClose()) {
        glGetError();

        window.newFrame();
        window.clearDevice(0.2f, 1.0f);
        window.clearDepth();
        window.applyCursorMode();

        {
            glm::vec3 move(0.0f);
            if(window.keyPressed(GLFW_KEY_R)) { camera = startCamera; }
            if(window.keyPressed(GLFW_KEY_W)) { move += glm::vec3( 0.0f, 0.0f,-1.0f); }
            if(window.keyPressed(GLFW_KEY_S)) { move += glm::vec3( 0.0f, 0.0f, 1.0f); }
            if(window.keyPressed(GLFW_KEY_A)) { move += glm::vec3(-1.0f, 0.0f, 0.0f); }
            if(window.keyPressed(GLFW_KEY_D)) { move += glm::vec3( 1.0f, 0.0f, 0.0f); }
            if(window.keyPressed(GLFW_KEY_SPACE)) { move += glm::vec3( 0.0f, 1.0f, 0.0f); }
            if(window.keyPressed(GLFW_KEY_LEFT_SHIFT)) { move += glm::vec3( 0.0f,-1.0f, 0.0f); }
            // if(window.keyPressed(GLFW_KEY_TAB)) { type = (type+1)%2; Sleep(300); std::out<<"type = "<<type<<"\n"; }
            camera.move(move, window.frame.dt);
        }

        // switch(type) {
        //     case 0: FBO .use(); break;
        //     case 1: FBO2.use(); break;
        // }
        // window.clearDevice(0.1f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(VAO);
          shader1.use();
              shader1.set("camTrans", camera.getMatrix());
              //shader1.set("camTrans", glm::mat4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1));
              glDrawArrays(GL_POINTS, viewSize.y*viewSize.z+viewSize.z+1, viewSize.x*viewSize.y*viewSize.z-2*viewSize.y*viewSize.z-2*viewSize.z-2);
          shader1.unuse();
        glBindVertexArray(0);

        // switch(type) {
        //     case 0: FBO .draw(); break;
        //     case 1: FBO2.draw(); break;
        // }

        window.swapBuffers();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}