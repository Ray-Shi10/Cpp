#include <vector>
#include <v2/Shader.h>
#include <v2/Canvas.h>
//#include <v2/Texture.h>
//#include <v2/Image.h>
#include <v2/Window.h>
//#include <v1/Function.h>
//#include <v1/Random.h>
#include <v2/VertexArray.h>
#include <v2/Camera.h>

//#include <GLFW/glfw3.h>
int main() {
    using std::vector;
    initGLFW(4,6);
    Window window("Test", glm::uvec2(800, 600), glm::vec2(1200, 800));/**/
    Window window2("@@@", glm::uvec2(800, 600), glm::vec2(1200, 800), 0);/*/
    glfwCreateWindow(800, 600, "Test2", nullptr, window.window.glfwWindow);/**/
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800,600,1000), 0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
    ShaderProgram shader(R"(
        layout(location = 0) in vec3 color;
        layout(location = 1) in vec3 position;
        out vec3 vColor;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(position, 1.0f);
            vColor = color;
        }
    )", R"(
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 1.0);
        }
    )");
    ShaderProgram shader2(R"(
        layout(location = 0) in vec3 color;
        layout(location = 1) in vec2 position;
        out vec3 vColor;
        void main() {
            gl_Position = vec4(position,0.0f, 0.0f);
            vColor = color;
        }
    )", R"(
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 1.0);
        }
    )");
    Buffer<glm::vec3, glm::vec3> cubeAttributes({
         //position                //color
        {{-0.5f, -0.5f, -0.5f},    {0.0f, 0.0f, 0.0f}}, 
        {{ 0.5f, -0.5f, -0.5f},    {1.0f, 0.0f, 0.0f}}, 
        {{-0.5f,  0.5f, -0.5f},    {0.0f, 1.0f, 0.0f}}, 
        {{ 0.5f,  0.5f, -0.5f},    {1.0f, 1.0f, 0.0f}}, 
        {{-0.5f, -0.5f,  0.5f},    {0.0f, 0.0f, 1.0f}}, 
        {{ 0.5f, -0.5f,  0.5f},    {1.0f, 0.0f, 1.0f}}, 
        {{-0.5f,  0.5f,  0.5f},    {0.0f, 1.0f, 1.0f}}, 
        {{ 0.5f,  0.5f,  0.5f},    {1.0f, 1.0f, 1.0f}},
    });
    Buffer<glm::vec2, glm::vec3> quadAttributes({
         //position                //color
        {{-0.5f, -0.5f},    {0.0f, 0.0f, 1.0f}}, 
        {{ 0.5f, -0.5f},    {1.0f, 0.0f, 0.0f}}, 
        {{-0.5f,  0.5f},    {0.0f, 1.0f, 0.0f}}, 
        {{ 0.5f,  0.5f},    {1.0f, 1.0f, 0.0f}}, 
    });
    float *tmp = (float*)cubeAttributes.dataPtr();
    /**
    Buffer<GLushort> indices({0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5});/*/
    Buffer<GLushort> indices({5, 4, 7, 6, 2, 4, 0, 5, 1, 7, 3, 2, 1, 0});/**/
    VertexArray VAO; VAO[indices](cubeAttributes);
    VertexArray VAO2; VAO2(quadAttributes);
    shader.use();
        shader.set("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)));
    //shader.unuse();
    window.setCursorMode(GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, make_function([&](GLFWwindow*, int key, int scancode, int action, int mods) {
        if(action == GLFW_PRESS) {
            switch(key) {
                case GLFW_KEY_ESCAPE:
                    window.window.active ^= 1;
                    std::cout << "Window active: " << window.window.active << std::endl;
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
        std::cout << "Mouse pos: " << xpos << ", " << ypos << std::endl;
        window.mouse.pos = glm::vec2(xpos, ypos);
        window.mouse.offset = window.mouse.pos - window.mouse.last;
        window.mouse.last = window.mouse.pos;
        if(window.window.active)
            camera.rotate(glm::vec3(window.mouse.offset.y, window.mouse.offset.x, 0.0f));
    }));
    glfwSetWindowFocusCallback(window, make_function([&](GLFWwindow*, int focused) {
        window.window.active = focused;
        //window.mouse.entering = true;
        std::out << "Window focus: " << focused << std::endl;
    }));
    glEnable(GL_DEPTH_TEST);
    while(window.shouldClose()) {
        //window.active();
        window.newFrame();
        window.clearDevice(0.2f, 1.0f);
        window.clearDepth();
        window.applyCursorMode();
        {
            glm::vec3 move(0.0f);
            if(window.keyPressed(GLFW_KEY_W)) { move += glm::vec3( 0.0f, 0.0f,-1.0f); }
            if(window.keyPressed(GLFW_KEY_S)) { move += glm::vec3( 0.0f, 0.0f, 1.0f); }
            if(window.keyPressed(GLFW_KEY_A)) { move += glm::vec3(-1.0f, 0.0f, 0.0f); }
            if(window.keyPressed(GLFW_KEY_D)) { move += glm::vec3( 1.0f, 0.0f, 0.0f); }
            if(window.keyPressed(GLFW_KEY_SPACE)) { move += glm::vec3( 0.0f, 1.0f, 0.0f); }
            if(window.keyPressed(GLFW_KEY_LEFT_SHIFT)) { move += glm::vec3( 0.0f,-1.0f, 0.0f); }
            camera.move(move, window.frame.dt);
        }
        VAO.bind();
            shader.use();
                shader.set("view", camera.getView());
                shader.set("projection", camera.getProj());
                glDrawElements(GL_TRIANGLE_STRIP, 14);
            //shader.unuse();
        //VAO.unbind();
        window.swapBuffers();
        glfwPollEvents();
        
        window2.active();
        window2.clearDevice(0.2f, 1.0f);
        VAO2.bind();
            shader2.use();
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            //shader2.unuse();
        //VAO2.unbind();
        window2.swapBuffers();
        //glfwPollEvents();
    }
    return 0;
}