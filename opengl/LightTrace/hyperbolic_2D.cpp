#include <vector>
#include <v2/Shader.h>
// #include <v2/Canvas.h>
#include <v2/Window.h>
// #include <v2/VertexArray.h>
// #include <v2/Camera.h>
// #include <complex>
#include <cmath>

int main() {
	using std::vector;
	initGLFW(4,6);
	Window window("Test", glm::uvec2(600, 600), glm::vec2(1200, 800));
  // Camera camera;
	// const Camera startCamera = camera;
	// window.setCursorMode(GLFW_CURSOR_DISABLED);

  // glfwSetCursorPosCallback(window, make_function([&](GLFWwindow*, double xpos, double ypos) {
  //   if(window.mouse.entering) {
  //     window.mouse.last = glm::vec2(xpos, ypos);
  //     window.mouse.entering = false;
  //   }
  //   std::debug << "Mouse pos: " << xpos << ", " << ypos << "\n";
  //   window.mouse.pos = glm::vec2(xpos, ypos);
  //   window.mouse.offset = window.mouse.pos - window.mouse.last;
  //   window.mouse.last = window.mouse.pos;
  //   if(window.window.active) {
  //     camera.rotate(window.mouse.offset.x * 0.01f);
  //   }
  // }));
  glfwSetKeyCallback(window, make_function([&](GLFWwindow*, int key, int, int action, int) {
    if(action == GLFW_PRESS) {
      switch(key) {
        case GLFW_KEY_ESCAPE:
          window.window.active ^= true;
          window.applyCursorMode();
          break;
        case GLFW_KEY_LEFT_CONTROL:
          window.close(); break;
        default: break;
      }
    }
  }));

  ShaderProgram shader1(R"(
    layout (location = 0) in vec2 aPos;
    // out vec2 uv;
    out vec3 color;
    // uniform vec2 iResolution;
    uniform mat3 camPosMat;

    void main() {
      // uv = aPos * iResolution / 500.0f;
      color = vec3(0.8);
      if(length(aPos) > 1000.0) color = vec3(1);
      vec3 pos = vec3(aPos, sqrt(dot(aPos,aPos)+1.0));
      pos = camPosMat * pos;
      gl_Position = vec4(pos.xy / pos.z, 0.0, 1.0);
    }
  )", R"(
    // in vec2 uv;
    in vec3 color;
    out vec4 FragColor;

    void main() {
      // if (length(uv) > 1.0) discard;
      
      // vec2 v1 = vec2( 1.0f,  1.0f),
      //      v2 = vec2(-1.0f,  2.0f),
      //      v3 = vec2( 1.5f, -1.0f);

      FragColor = vec4(color, 1.0);
    }
  )"), shader_circle(R"(
    layout (location = 0) in vec2 aPos;
    out vec2 pos;
    void main() {
      pos = aPos;
      gl_Position = vec4(aPos, 0.0, 1.0);
    }
  )", R"(
    out vec4 FragColor;
    in vec2 pos;
    vec2 cMult(vec2 a, vec2 b) {
      return vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x);
    }
    vec2 cDiv(vec2 a, vec2 b) {
      return cMult(a,vec2(b.x,-b.y)) / dot(b,b);
    }
    void main() {
      if(length(pos) > 1.0f) {
        FragColor = vec4(1);
        return;
      }
      // vec2 model = pos / sqrt(1-dot(pos,pos));
      // vec3 minsk = vec3(model, sqrt(dot(model,model)+1));
      // vec2 pjl = minsk.xy / (minsk.z+1);
      vec2 pjl = pos / (1 + sqrt(1-dot(pos,pos)));
      vec2 hPlane = cDiv(vec2(1,0)+pjl, vec2(1,0)-pjl);
      hPlane = vec2(-hPlane.y, hPlane.x);
      hPlane = fract(hPlane);
      hPlane *= 2.0;
      hPlane -= 1.0;
      hPlane = -abs(hPlane);
      float grid = step(-0.1, hPlane.x) + step(-0.1, hPlane.y);
      FragColor = vec4(vec3(grid), 1);
    }
  )");

  GLuint vao[2], vbo[2];
  glGenVertexArrays(2, vao);
  glGenBuffers(2, vbo);
  glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
      float quadVertices[] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
      };
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
      float vertices[] = {
        -1.0f, -2.0f,
        1.5f, -1.0f,
        -1.0f,  2.5f,
        3000,  1000,
      };
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glm::mat3 camPosMat(1.0f);

  while(!window.shouldClose()) {
    window.newFrame();
    window.clearDevice(0.0f, 1.0f);
    window.clearDepth();
    window.applyCursorMode();
    window.getSize();
    window.viewPort();

    {
      glm::vec2 move(0.0f);
      if(window.keyPressed(GLFW_KEY_W)) { move.y += 1; }
      if(window.keyPressed(GLFW_KEY_S)) { move.y -= 1; }
      if(window.keyPressed(GLFW_KEY_A)) { move.x -= 1; }
      if(window.keyPressed(GLFW_KEY_D)) { move.x += 1; }
      move *= window.frame.dt;
      const float cx = coshf(move.x);
      const float cy = coshf(move.y);
      const float sx = sinhf(move.x);
      const float sy = sinhf(move.y);
      camPosMat =
      glm::mat3(
        cx, 0, sx,
         0, 1,  0,
        sx, 0, cx
      ) * 
      glm::mat3(
         1,  0,  0,
         0, cy, sy,
         0, sy, cy
      ) * camPosMat;
    }

    // shader1.use();
    //   shader1.set("iResolution", glm::vec2(window.window.size.x, window.window.size.y));
    //   shader1.set("camPosMat", camPosMat);
    //   glBindVertexArray(vao[0]);
    //     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      // glBindVertexArray(0);
    shader_circle.use();
      glBindVertexArray(vao[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      // glBindVertexArray(0);
    // Shader::clear();

    window.swapBuffers();
    glfwPollEvents();
  }
	return 0;
}