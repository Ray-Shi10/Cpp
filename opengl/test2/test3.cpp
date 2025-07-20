//#define DEBUG
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

int main() {
    using std::vector;
    initGLFW(4,6);
    Window window("Test", glm::uvec2(800, 600), glm::vec2(1200, 800));/**/
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800,600,1000), 0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
    ShaderProgram shader(R"(
        layout(location = 0) in vec3 position;
        //out vs2gs { uint blockType; } attrib;
        void main() {
            gl_Position = vec4(position, 1.0f);
        }
    )", R"(
        layout (points) in;
        //in vs2gs { uint blockType; } attribs[];
        layout (triangle_strip, max_vertices = 14) out;
        out vec3 fragColor;
        uniform mat4 camTrans;
        void addPoint(vec3 offset, vec3 color) {
            gl_Position = camTrans * (gl_in[0].gl_Position + vec4(offset/2, 0.0f));
            fragColor = color;
            EmitVertex();
        }
        void main() {
            // 0 1 2 3 7 1 5 0 4 2 6 7 4 5
            addPoint(vec3( 1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f)); // 0
            addPoint(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f)); // 1
            addPoint(vec3( 1.0f,-1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f)); // 2
            addPoint(vec3(-1.0f,-1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)); // 3
            addPoint(vec3(-1.0f,-1.0f,-1.0f), vec3(0.0f, 0.0f, 0.0f)); // 7
            addPoint(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f)); // 1
            addPoint(vec3(-1.0f, 1.0f,-1.0f), vec3(0.0f, 1.0f, 0.0f)); // 5
            addPoint(vec3( 1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f)); // 0
            addPoint(vec3( 1.0f, 1.0f,-1.0f), vec3(1.0f, 1.0f, 0.0f)); // 4
            addPoint(vec3( 1.0f,-1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f)); // 2
            addPoint(vec3( 1.0f,-1.0f,-1.0f), vec3(1.0f, 0.0f, 1.0f)); // 6
            addPoint(vec3(-1.0f,-1.0f,-1.0f), vec3(0.0f, 0.0f, 0.0f)); // 7
            addPoint(vec3( 1.0f, 1.0f,-1.0f), vec3(1.0f, 1.0f, 0.0f)); // 4
            addPoint(vec3(-1.0f, 1.0f,-1.0f), vec3(0.0f, 1.0f, 0.0f)); // 5
            EndPrimitive();
        }
    )", R"(
        in vec3 fragColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(fragColor, 1.0);
        }
    )");
    Buffer<glm::vec3> cubeAttributes({
          //position             
        {{-2.0f, -2.0f, -2.0f}}, 
        {{ 2.0f, -2.0f, -2.0f}}, 
        {{-2.0f,  2.0f, -2.0f}}, 
        {{ 2.0f,  2.0f, -2.0f}}, 
        {{-2.0f, -2.0f,  2.0f}}, 
        {{ 2.0f, -2.0f,  2.0f}}, 
        {{-2.0f,  2.0f,  2.0f}}, 
        {{ 2.0f,  2.0f,  2.0f}}, 
    });
    VertexArray VAO; VAO(cubeAttributes);

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

    while(window.shouldClose()) {
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
                shader.set("camTrans", camera.getMatrix());
                glDrawArrays(GL_POINTS, 0, 8);
            //shader.unuse();
        //VAO.unbind();

        window.swapBuffers();
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}