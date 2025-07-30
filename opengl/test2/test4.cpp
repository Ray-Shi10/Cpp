#include <vector>
#include <v2/Shader.h>
// #include <v2/Canvas.h>
#include <v2/Window.h>
//#include <v2/VertexArray.h>
#include <v2/Camera.h>

int main() {
  using std::vector;
  initGLFW(4,6);
  Window window("Test", glm::uvec2(800, 600), glm::vec2(1200, 800));
  Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800,600,1000),
          0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
  const Camera startCamera = camera;
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

  ShaderProgram shader1(R"(
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in uint blockType;
    out vs2gs {
      uint blockType;
    } attrib;
    void main() {
      gl_Position = vec4(aPos, 1.0f);
      attrib.blockType = blockType;
    })", R"(
    layout (points) in;
    in vs2gs {
      uint blockType;
    } attribs[];
    out gs2fs {
      vec2 texCoord;
      flat uint blockType;
    } attrib;
    layout (triangle_strip, max_vertices = 14) out;
    uniform mat4 camTrans;
    void addPoint(vec3 offset, vec2 texCoord) {
      gl_Position = camTrans * ( gl_in[0].gl_Position + vec4(offset/2, 0.0f) );
      attrib.blockType = attribs[0].blockType;
      attrib.texCoord = texCoord;
      EmitVertex();
    }
    void main() {
      if(attribs[0].blockType == 0U) return;
      addPoint(vec3( 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));
      addPoint(vec3(-1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f));
      addPoint(vec3( 1.0f, 1.0f,-1.0f), vec2(0.0f, 1.0f));
      addPoint(vec3(-1.0f, 1.0f,-1.0f), vec2(1.0f, 1.0f));
      addPoint(vec3(-1.0f,-1.0f,-1.0f), vec2(1.0f, 2.0f));
      addPoint(vec3(-1.0f, 1.0f, 1.0f), vec2(2.0f, 1.0f));
      addPoint(vec3(-1.0f,-1.0f, 1.0f), vec2(2.0f, 2.0f));
      addPoint(vec3( 1.0f, 1.0f, 1.0f), vec2(3.0f, 1.0f));
      addPoint(vec3( 1.0f,-1.0f, 1.0f), vec2(3.0f, 2.0f));
      addPoint(vec3( 1.0f, 1.0f,-1.0f), vec2(4.0f, 1.0f));
      addPoint(vec3( 1.0f,-1.0f,-1.0f), vec2(4.0f, 2.0f));
      addPoint(vec3(-1.0f,-1.0f,-1.0f), vec2(5.0f, 2.0f));
      addPoint(vec3( 1.0f,-1.0f, 1.0f), vec2(4.0f, 3.0f));
      addPoint(vec3(-1.0f,-1.0f, 1.0f), vec2(5.0f, 3.0f));
      EndPrimitive();
    })", R"(
    out vec4 FragColor;
    in gs2fs {
      vec2 texCoord;
      flat uint blockType;
    } attrib;
    uniform sampler2DArray texArray;
    uniform vec2 textureSize;
    void main() {
      // Debug: output solid color to check geometry
      // FragColor = vec4(1,0,0,1);
      // Sample from texture array
      vec2 uv = attrib.texCoord / textureSize;
      float layerIndex = float(attrib.blockType-1U);
      FragColor = vec4(vec3(texture(texArray, vec3(uv, layerIndex))),1.0f);
    })"
  );

  constexpr glm::ivec3 viewCoordMax(3,2,3) , viewCoordMin(-viewCoordMax),
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
        if(x==0 || y==0 || z==0 || x==viewSize.x-1 || y==viewSize.y-1 || z==viewSize.z-1) {
          blockTypes[x][y][z] = 0U;
        } else if(y==3) {
          blockTypes[x][y][z] = 1U;
        } else {
          blockTypes[x][y][z] = 2U;
        }
      }
    }
  }

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
      glBufferSubData(GL_ARRAY_BUFFER, (&blockTypes[2][2][2]-&blockTypes[0][0][0])*sizeof(GLenum), sizeof(GLenum), _tmp);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 4, 12, 2);
  const unsigned char imgs[2][3*4*12] = {
    {51,105,30,104,159,56,51,105,30,104,159,56,104,159,56,51,105,30,104,159,56,51,105,30,51,105,30,51,105,30,104,159,56,51,105,30,104,159,56,104,159,56,51,105,30,104,159,56,104,159,56,121,85,72,104,159,56,121,85,72,62,39,35,93,64,55,121,85,72,62,39,35,93,64,55,62,39,35,93,64,55,62,39,35,62,39,35,93,64,55,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55},
    {62,39,35,62,39,35,93,64,55,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55,62,39,35,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,62,39,35,93,64,55},
  };
  for (int i = 0; i < 2; i++) {
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 4, 12, 1, GL_RGB, GL_UNSIGNED_BYTE, imgs[i]);
  }
  shader1.use();
    shader1.set("texArray", 0);
    shader1.set("textureSize", glm::vec2(1, 3));
  shader1.unuse();

  while(!window.shouldClose()) {
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

    window.swapBuffers();
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}