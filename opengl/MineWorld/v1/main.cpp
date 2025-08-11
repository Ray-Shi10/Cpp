#include <vector>
#include <v2/Shader.h>
// #include <v2/Canvas.h>
#include <v2/Window.h>
//#include <v2/VertexArray.h>
#include <v2/Camera.h>

int main() {
  using std::vector;
  initGLFW(4,6);
  glm::vec2 screenSize(2000, 800);
  Window window("Test", screenSize, glm::vec2(1000, 800));
  Camera camera(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800,600,1000),
          0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
  glViewport(0, 0, screenSize.x, screenSize.y);
  glm::vec3 speed;
  const Camera startCamera = camera;
  window.setCursorMode(GLFW_CURSOR_DISABLED);

  glfwSetKeyCallback(window, make_function([&](GLFWwindow*, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
      switch(key) {
        case GLFW_KEY_ESCAPE:
          window.window.active ^= 1;
          // glfwFocusWindow(window.window.active ? window.window.glfwWindow : nullptr);
          // std::cout << "Window active: " << window.window.active << "\n";
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
    // std::debug << "Window focus: " << focused << "\n";
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
      float depth = gl_Position.w;
      gl_Position /= depth;
      gl_Position.x /= 20.0f;
      gl_Position.y /= 8.0f;
      gl_Position.xy *= 8.0f;
      gl_Position *= depth;
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

  constexpr glm::ivec3 viewCoordMax(3,3,3) , viewCoordMin(-viewCoordMax),
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
        } else if(y==4 && x==2 && z==2) {
          blockTypes[x][y][z] = 1U;
        } else if(y==4 || y==5) {
          blockTypes[x][y][z] = 0U;
        } else {
          blockTypes[x][y][z] = 2U;
        }
      }
    }
  }

  GLuint VAO ; glGenVertexArrays(1, &VAO );
  GLuint VBO ; glGenBuffers     (1, &VBO );
  GLuint VBO2; glGenBuffers     (1, &VBO2);

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

  // --- FBO with Depth Attachment ---
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // Create color texture
  GLuint fboColor;
  glGenTextures(1, &fboColor);
  glBindTexture(GL_TEXTURE_2D, fboColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColor, 0);

  // Create depth renderbuffer
  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  // glGenRenderbuffers(1, &fboDepth);
  // glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
  // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepth);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLuint quadVAO = 0, quadVBO = 0;
  float quadVertices[] = {
      // positions   // texCoords
       1.0f,  1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
  };
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // Simple shader for displaying the FBO texture
  ShaderProgram screenShader(R"(
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
      gl_Position = vec4(aPos, 0.0, 1.0);
      TexCoord = aTexCoord;
    })", R"(
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D screenTexture;
    uniform sampler2D screenDepth;
    void main() {
      vec3 color = texture(screenTexture, TexCoord).rgb;
      float depth = texture(screenDepth, TexCoord).r;
      // color = vec3(1.0f) - color;
      // FragColor = vec4(vec3(depth), 1.0f);
      FragColor = vec4(color, 1.0f);
    })"
  );
  screenShader.use();
    screenShader.set("screenTexture", 1);
    screenShader.set("screenDepth", 2);
  screenShader.unuse();

  float cursor[] = {
    -1.0f, 0.0f,    1.0f, 0.0f,
     0.0f, 1.0f,    0.0f,-1.0f,
  };
  GLuint VAO_cursor;
  glGenVertexArrays(1, &VAO_cursor);
  glBindVertexArray(VAO_cursor);
  GLuint VBO_cursor;
  glGenBuffers(1, &VBO_cursor);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_cursor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cursor), cursor, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  ShaderProgram cursorShader(R"(
    layout(location = 0) in vec2 pos;
    uniform vec2 screenSize;
    void main() {
      // vec2 screenSize = vec2(1200.0f, 800.0f); // Replace with actual screen size
      gl_Position = vec4(pos/screenSize*min(screenSize.x,screenSize.y)/25.0f, 0.0f, 1.0f);
    })", R"(
    out vec4 FragColor;
    void main() {
      FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    })"
  );
  glLineWidth(2.0f);

  while(!window.shouldClose()) {
    window.applyCursorMode();
    window.newFrame();
    // window.clearDevice(0.2f, 1.0f);
    // window.clearDepth();

    {
      glm::vec3 force(0.0f, -0.05f, 0.0f);
      glm::vec3 moveF(0);
      static constexpr float moveSpeed = 0.015f;
      static constexpr float jumpSpeed = 90.0f;
      if(window.keyPressed(GLFW_KEY_R)) { camera = startCamera; speed = glm::vec3(0.0f); }
      if(window.keyPressed(GLFW_KEY_W)) { moveF += glm::vec3(0,0,-moveSpeed); }
      if(window.keyPressed(GLFW_KEY_S)) { moveF += glm::vec3(0,0, moveSpeed); }
      if(window.keyPressed(GLFW_KEY_A)) { moveF += glm::vec3(-moveSpeed,0,0); }
      if(window.keyPressed(GLFW_KEY_D)) { moveF += glm::vec3( moveSpeed,0,0); }
      if(window.keyPressed(GLFW_KEY_SPACE)) {
        // std::cout << camera.pos.y << "\n";
        if(camera.pos.y < 2.50001f) {
          moveF += glm::vec3(0,jumpSpeed,0);
          // std::cout << camera.pos.y << " Jumping!\n";
        } else {
          // std::cout << "You can't jump while in the air!\n";
        }
      }
      // if(window.keyPressed(GLFW_KEY_LEFT_SHIFT)) { moveF += glm::vec3( 0.0f,-1.0f, 0.0f); }
      // moveF /= 18.0f;
      moveF = glm::vec3(glm::rotateYl(camera.dir.y) * glm::vec4(moveF, 1.0f));
      // if(window.keyPressed(GLFW_KEY_LEFT_CONTROL)) {
      //   std::cout << "";
      // }
      // std::cout << glm::toString(force) << "\n" << glm::toString(moveF) << "\n" << glm::toString(speed) << "\n\n";
      force += moveF;
      speed += force;
      speed *= 0.997f; //摩擦力
      // if(glm::length(speed) > 10.0f) {
      //   speed = glm::normalize(speed) * 10.0f;
      // }
      // camera.move_s(speed * window.frame.dt);
      camera.pos += speed * window.frame.dt;
    }
    if(camera.pos.y < 2.5f) {
      camera.pos.y = 2.5f;
      //camera.dir.y = 0.0f;
    }

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);
      shader1.use();
        shader1.set("camTrans", camera.getMatrix());
        //shader1.set("camTrans", glm::mat4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1));
        glDrawArrays(GL_POINTS, viewSize.y*viewSize.z+viewSize.z+1, viewSize.x*viewSize.y*viewSize.z-2*viewSize.y*viewSize.z-2*viewSize.z-2);
      shader1.unuse();
    glBindVertexArray(0);
    float _midDepth[1];
    glReadPixels(screenSize.x/2, screenSize.y/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, _midDepth);
    // float midDepth = camera.znear + (camera.zfar-camera.znear)*_midDepth[0];
    float midDepth = (2.0 * camera.znear * camera.zfar) / (camera.zfar + camera.znear - (_midDepth[0] * 2 - 1) * (camera.zfar - camera.znear));
    // printf("Mid Depth: %03.3f ( %.4f )\n", midDepth, _midDepth[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // After rendering to FBO, render its color texture to the screen
    // --- Add after glBindFramebuffer(GL_FRAMEBUFFER, 0); ---
    // Setup a simple fullscreen quad VAO/VBO (once, outside the loop)
    window.clearDevice(0.2f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(quadVAO);
      screenShader.use();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fboColor);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, rbo);
        // glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      screenShader.unuse();
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO_cursor);
      cursorShader.use();
        // cursorShader.set("screenSize", glm::vec2(window.window.size.x, window.window.size.y));
        cursorShader.set("screenSize", screenSize);
        glDrawArrays(GL_LINES, 0, 4);
      cursorShader.unuse();
    glBindVertexArray(0);

    window.swapBuffers();
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}