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
  Camera camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(800,600,1000),
          0.01f, 1000, 3, 0.005f, 0);
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
          camera.pos = glm::vec3(0, 0, 0);
          camera.dir = glm::vec3(0, 0, 0);
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
      camera.rotate(glm::vec3(window.mouse.offset.y, window.mouse.offset.x, 0));
  }));
  glfwSetWindowFocusCallback(window, make_function([&](GLFWwindow*, int focused) {
    window.window.active = focused;
    //window.mouse.entering = true;
    std::debug << "Window focus: " << focused << "\n";
  }));

  glEnable(GL_DEPTH_TEST);

  ShaderProgram shader1(R"(
    layout(location = 0) in vec3 iPos;
    layout(location = 1) in uint iBlockType;
    layout(location = 2) in uint iFaceBlocked;
    flat out uint blockType;
    flat out uint faceBlocked;
    void main() {
      gl_Position = vec4(iPos, 1);
      blockType = iBlockType;
      faceBlocked = iFaceBlocked;
    })", R"(
    layout(points) in;
    flat in uint blockType[];
    flat in uint faceBlocked[];
    out gs2fs {
      vec2 texCoord;
      flat uint blockType;
    } attrib;
    layout(triangle_strip, max_vertices = 14) out;
    uniform mat4 camTrans;

    // Helper: test if face is blocked
    bool isBlocked(uint mask, int face) {
      return false;
      return (mask & (1U << face)) != 0U;
    }

    void addPoint(vec3 offset, vec2 texCoord) {
      gl_Position = camTrans * ( gl_in[0].gl_Position + vec4(offset/2, 0) );
      attrib.blockType = blockType[0];
      attrib.texCoord = texCoord;
      EmitVertex();
    }

    // Geometry Shader: fixed texCoord mapping for each face
    void main() {
      if(blockType[0] == 0U) return;
      uint mask = faceBlocked[0];
      // +Y face
      if(!isBlocked(mask, 2)) {
        addPoint(vec3( 1, 1, 1), vec2(1, 0));
        addPoint(vec3(-1, 1, 1), vec2(0, 0));
        addPoint(vec3( 1, 1,-1), vec2(1, 1));
        addPoint(vec3(-1, 1,-1), vec2(0, 1));
        EndPrimitive();
      }
      // -Z face
      if(!isBlocked(mask, 5)) {
        addPoint(vec3( 1, 1,-1), vec2(1, 1));
        addPoint(vec3(-1, 1,-1), vec2(0, 1));
        addPoint(vec3( 1,-1,-1), vec2(1, 2));
        addPoint(vec3(-1,-1,-1), vec2(0, 2));
        EndPrimitive();
      }
      // -Y face
      if(!isBlocked(mask, 3)) {
        addPoint(vec3( 1,-1,-1), vec2(1, 2));
        addPoint(vec3(-1,-1,-1), vec2(0, 2));
        addPoint(vec3( 1,-1, 1), vec2(1, 3));
        addPoint(vec3(-1,-1, 1), vec2(0, 3));
        EndPrimitive();
      }
      // +X face
      if(!isBlocked(mask, 0)) {
        addPoint(vec3( 1, 1, 1), vec2(2, 1));
        addPoint(vec3( 1,-1, 1), vec2(1, 1));
        addPoint(vec3( 1, 1,-1), vec2(2, 2));
        addPoint(vec3( 1,-1,-1), vec2(1, 2));
        EndPrimitive();
      }
      // +Z face
      if(!isBlocked(mask, 4)) {
        addPoint(vec3(-1, 1, 1), vec2(3, 1));
        addPoint(vec3( 1, 1, 1), vec2(2, 1));
        addPoint(vec3(-1,-1, 1), vec2(3, 2));
        addPoint(vec3( 1,-1, 1), vec2(2, 2));
        EndPrimitive();
      }
      // -X face
      if(!isBlocked(mask, 1)) {
        addPoint(vec3(-1, 1,-1), vec2(4, 1));
        addPoint(vec3(-1, 1, 1), vec2(3, 1));
        addPoint(vec3(-1,-1,-1), vec2(4, 2));
        addPoint(vec3(-1,-1, 1), vec2(3, 2));
        EndPrimitive();
      }
    }
    )", R"(
    out vec4 FragColor;
    in gs2fs {
      vec2 texCoord;
      flat uint blockType;
    } attrib;
    uniform sampler2DArray texArray;
    uniform vec2 textureSize;
    void main() {
      vec2 texCoord = attrib.texCoord;
      while(texCoord.x > 1.0f) {
        FragColor = vec4(1); return;
        texCoord.x -= 1.0f;
      }
      vec2 uv = texCoord / textureSize;
      float layerIndex = float(attrib.blockType - 1U);
      FragColor = texture(texArray, vec3(uv, layerIndex));
      if(FragColor.r < 0.1f) FragColor=vec4(1);
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

  unsigned char faceBlocked[viewSize.x][viewSize.y][viewSize.z] = {0};

  GLuint VAO; glGenVertexArrays(1, &VAO);
  GLuint VBO[3]; glGenBuffers  (3,  VBO);
  glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
      ;
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(blockTypes), blockTypes, GL_STATIC_DRAW);
      ;
      glEnableVertexAttribArray(1);
      glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(GLenum), (void*)(0));
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(faceBlocked), faceBlocked, GL_STATIC_DRAW);
      ;
      glEnableVertexAttribArray(2);
      glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(unsigned char), (void*)(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
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
    window.clearDevice(0.2f, 1);
    window.clearDepth();
    window.applyCursorMode();

    {
      glm::vec3 move(0);
      if(window.keyPressed(GLFW_KEY_R)) { camera = startCamera; }
      if(window.keyPressed(GLFW_KEY_W)) { move += glm::vec3( 0, 0,-1); }
      if(window.keyPressed(GLFW_KEY_S)) { move += glm::vec3( 0, 0, 1); }
      if(window.keyPressed(GLFW_KEY_A)) { move += glm::vec3(-1, 0, 0); }
      if(window.keyPressed(GLFW_KEY_D)) { move += glm::vec3( 1, 0, 0); }
      if(window.keyPressed(GLFW_KEY_SPACE)) { move += glm::vec3( 0, 1, 0); }
      if(window.keyPressed(GLFW_KEY_LEFT_SHIFT)) { move += glm::vec3( 0,-1, 0); }
      camera.move(move, window.frame.dt);
    }

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