#include <vector>
#include <v2/Shader.h>
#include <v2/Canvas.h>
#include <v2/Texture.h>
//#include <v2/Image.h>
#include <v2/Window.h>
#include <v1/Function.h>
#include <v1/Random.h>
#include <v2/VertexArray.h>

#include <GLFW/glfw3.h>

int main() {
    using std::vector;
    //Window window(800, 600, "Test");
    initGLFW(4,6);
    auto *window = glfwCreateWindow(800, 600, "Test");
    initGLAD();
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    });
    ShaderProgram shader(R"(
        #version 460 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 color;
        out vec2 vColor;
        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
            vColor = color;
        }
    )", R"(
        #version 460 core
        in vec2 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 0.0, 1.0);
        }
    )");
    vector<float> quadPositions({
        -1, -1, 
         1, -1, 
        -1,  1, 
         1,  1,
    });
    vector<float> quadColors({
        0, 0, 
        1, 0, 
        0, 1, 
        1, 1,
    });
    VertexArray VAO;/**/
    VAO(quadPositions, 2);
    VAO(quadColors, 2);/*/
    VAO.bind();
        GLuint VBO[2];
        glGenBuffers(2, VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, quadPositions.size() * sizeof(float), quadPositions.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
            glBufferData(GL_ARRAY_BUFFER, quadColors.size() * sizeof(glm::vec2), quadColors.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    VAO.unbind();/**/
    while(!glfwWindowShouldClose(window)) {
        VAO.bind();
            shader.use();
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            shader.unuse();
        VAO.unbind();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}