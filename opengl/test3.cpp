#include <initGL.h>
#include <Camera.h>
#include <Shader.h>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(SCR_WIDTH, SCR_HEIGHT, 800), 0.1f, 100.0f, 0.1f, 0.005f, 0.0f);

bool firstMouse = true;
bool pause = false;
float lastX = 0;
float lastY = 0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
        std::cout << "focused: " << focused << "\n";
        if(focused == GLFW_FALSE) {
            pause = true;
        }
    });
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            if(pause) {
                //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                if(mods && GLFW_MOD_SHIFT || mods && GLFW_MOD_CONTROL || mods && GLFW_MOD_ALT) {
                    glfwSetWindowShouldClose(window, true);
                } else {
                    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            pause = !pause;
        }
    });

    // tell GLFW to capture our mouse
    //----------------------------------------------------------------------------------------------------------------------------Cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << "\n";
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader(
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
"}",
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 Color;\n"
"void main() {\n"
"	 FragColor = vec4(Color,0.0f,1.0f);\n"
"}");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, //0
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, //1
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, //2
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, //3
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, //4
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, //5
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, //6
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f  //7
    };
    unsigned int indices[] = {
        0, 1, 2,    1, 2, 3,
        4, 5, 6,    5, 6, 7,
        0, 1, 4,    1, 4, 5,
        2, 3, 6,    3, 6, 7,
        0, 2, 4,    2, 4, 6,
        1, 3, 5,    3, 5, 7
    };
    // world space positions of our cubes
    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f,  0.0f,  10.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, 12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, 7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, 1.5f)
    }; const short cubeamount = cubePositions.size();
    //glm::mat4(1.0f)*glm::vec4(1);
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

    //*
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);//*/

    // render loop
    // -----------
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
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // activate shader
        ourShader.use();

        //*
        ourShader.setMat4("projection", camera.getProjectionMatrix());/*/
        ourShader.setMat4("projection", glm::mat4(1.0f));//*/
        //ourShader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));

        //*
        ourShader.setMat4("view", camera.getViewMatrix());/*/
        ourShader.setMat4("view", glm::mat4(1.0f));//*/

        //ourShader.setMat4("view", glm::translate(glm::mat4(1.0f), -camera.pos));

        // render boxes
        glBindVertexArray(VAO);//*
        for (unsigned int i = 0; i < cubeamount; i++)/*/
        int i=0;//*/
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            /*
            glDrawArrays(GL_TRIANGLES, 0, 36);/*/
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);//*/
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(15);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    glm::vec3 cameraMove(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraMove += glm::vec3(0.0f, 0.0f, -1.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraMove += glm::vec3(0.0f, 0.0f, 1.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraMove += glm::vec3(-1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraMove += glm::vec3(1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraMove += glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraMove += glm::vec3(0.0f, -1.0f, 0.0f);
    //if(glm::length(cameraMove) > 0.0f)
    //    std::cout << "cameraMove: " << cameraMove.x << ", " << cameraMove.y << ", " << cameraMove.z << "\ndeltaTime: " << deltaTime << " " << static_cast<float>(deltaTime*100) << "\n";
    camera.move(cameraMove, static_cast<float>(deltaTime*100));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width; SCR_HEIGHT = height;
    firstMouse = true;
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 1; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    camera.rotate(glm::vec3(yoffset, -xoffset, 0.0f));
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //std::cout << "scroll_callback: " << yoffset << "\n";
    camera.view.z -= static_cast<float>(yoffset)*1000*camera.sens_zoom;
    if (camera.view.z < 100.0f)
        camera.view.z = 100.0f;
    if (camera.view.z > 1000.0f)
        camera.view.z = 1000.0f;
}