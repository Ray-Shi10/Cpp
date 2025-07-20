#include <initGL.h>
#include <Camera.h>
#include <Shader.h>
#include <Window.h>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(SCR_WIDTH, SCR_HEIGHT, 800), 0.1f, 100.0f, 0.1f, 0.005f, 0.0f);

bool firstMouse = true;
bool pause = false;
float lastX = 0;
float lastY = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    initGLFW(4, 6);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL");
    glfwSetWindowPos(window, 1500, 1000);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
        if(focused == GLFW_FALSE) {
            pause = true;
        }
    });
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            if(!pause) {
                if(mods & (GLFW_MOD_SHIFT|GLFW_MOD_CONTROL|GLFW_MOD_ALT) ) {
                    glfwSetWindowShouldClose(window, true);
                }
            }
            pause = !pause;
        }
    });
    
    glfwSetCursor(window, GLFW_CURSOR_DISABLED);

    initGLAD();

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
        if(pause) {
            if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            glfwWaitEvents();
            continue;
        } else if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        shader1.use();

        //*
        shader1.set("projection", camera.getProjectionMatrix());/*/
        shader1.set("projection", glm::mat4(1.0f));//*/
        //shader1.set("projection", glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));

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

void processInput(GLFWwindow *window) {
    glm::vec3 cameraMove(0.0f);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraMove += glm::vec3( 0.0f, 0.0f,-1.0f);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraMove += glm::vec3( 0.0f, 0.0f, 1.0f);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraMove += glm::vec3(-1.0f, 0.0f, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraMove += glm::vec3( 1.0f, 0.0f, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraMove += glm::vec3( 0.0f, 1.0f, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraMove += glm::vec3( 0.0f,-1.0f, 0.0f);/*
    if(glm::lengthSQ(cameraMove) > 0.0f) {
        std::cout << "Camera move: " << glm::toString(cameraMove) << "\n";
        std::cout << "----------------------------\n";
    }//*/
    camera.move(cameraMove, static_cast<float>(deltaTime*100));
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(SCR_WIDTH, SCR_HEIGHT, 800), 0.1f, 100.0f, 0.1f, 0.005f, 0.0f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    SCR_WIDTH = width; SCR_HEIGHT = height;
    firstMouse = true;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;
    /*
    if(glm::lengthSQ(glm::vec2(xoffset, yoffset)) > 0.0f)
        std::cout << "Mouse Offset: " << xoffset << " " << yoffset << "\n";//*/

    camera.rotate(glm::vec3(yoffset, xoffset, 0.0f));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    //now disabled (because sens_zoom is 0.0f)
    camera.view.z -= static_cast<float>(yoffset) * 1000 * camera.sens_zoom;
    if(camera.view.z < 100.0f)
        camera.view.z = 100.0f;
    if(camera.view.z > 1000.0f)
        camera.view.z = 1000.0f;
}