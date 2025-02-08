#include <initGL.h>
#include <Camera.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() { std::cout << "Start program.\n";
    initGLFW(4, 6);
    //initErrorCallback();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL");
    glfwMakeContextCurrent(window);// glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);/*
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        std::cout << "Cursor position: " << xpos << " " << ypos << std::endl;
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
        std::cout << "Cursor " << (entered ? "entered" : "left") << " window\n";
    });
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        std::cout << "Window closed\n";
    });
    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
        std::cout << "Window " << (focused ? "focused" : "unfocused") << std::endl;
    });
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        std::cout << "Key " << key << " " << scancode << " " << action << " " << mods << std::endl;
    });
    glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
        std::cout << "Char " << codepoint << std::endl;
    });
    glfwSetErrorCallback([](int error, const char* description) {
        std::cout << "Error " << error << ": " << description << std::endl;
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        std::cout << "Scroll " << xoffset << " " << yoffset << std::endl;
    });
    glfwSetWindowContentScaleCallback(window, [](GLFWwindow* window, float xScale, float yScale) {
        std::cout << "Content scale " << xScale << " " << yScale << std::endl;
    });//*/
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        printf("Key message:");
        switch(action) {
            case GLFW_PRESS:
                printf("Pressed");
                break;
            case GLFW_RELEASE:
                printf("Released");
                break;
            case GLFW_REPEAT:
                printf("Repeated");
                break;
        }
        printf("\n    key: ");
        if(key <= 100) {
            printf("%c", key);
        } else {
            printf("%d", key);
        }
        printf("\n    scancode: %d\n    mods: %d\n", scancode, mods);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        std::cout << "Cursor position: " << xpos << " " << ypos << "\n";
    });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    initGLAD();
    //std::cout << GLVersion.major << " " << GLVersion.minor << std::endl;
    Camera cam(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(SCR_WIDTH, SCR_HEIGHT, 300), 0.1f, 100.0f, 0.05f, 0.1f, 0.1f);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << "\033c";
    return 0;
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}