#include <vector>
#include <v2/Shader.h>
// #include <v2/Canvas.h>
#include <v2/Window.h>
// #include <v2/VertexArray.h>
#include <v2/Camera.h>

int main() {
	using std::vector;
	initGLFW(4,6);
	Window window("Test", glm::uvec2(800, 600), glm::vec2(1200, 800));
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800,600,1000),
					0.01f, 1000.0f, 3.0f, 0.005f, 0.0f);
	glm::vec3 speed;
	const Camera startCamera = camera;
	window.setCursorMode(GLFW_CURSOR_DISABLED);

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
    out vec2 fragCoord;

    void main() {
        fragCoord = aPos;
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
  )", R"(
    out vec4 FragColor;
    in vec2 fragCoord;

    uniform vec2 iResolution;
    uniform float iTime;
    uniform mat4 iCamera;

    #define PI 3.14159265359
    #define MAX_DEPTH 5  // 最大反射深度

    struct Ray {
      vec3 origin;    // 射线起点
      vec3 direction; // 射线方向
    };

    struct Sphere {
      vec3 center;    // 球心
      float radius;   // 半径
      vec3 color;     // 材质
    };

    struct HitInfo {
      float dis;          // 距离
      vec3 point;         // 碰撞点
      vec3 normal;        // 法向量
      vec3 color;         // 颜色
    //   bool portal;
    //   Ray ray;            // 射线
    };
    void initHit(out HitInfo hit) {
      hit.dis = 1e9;
      hit.color = vec3(0.0);
    //   hit.ray.origin = vec3(0.0);
    //   hit.ray.direction = vec3(0.0, 0.0, 0.0);
    }

    HitInfo intersect(Ray ray, Sphere sphere) {
      HitInfo hit;  initHit(hit);
      
      vec3 oc = ray.origin - sphere.center;
      float b = dot(oc, ray.direction);
      float c = dot(oc, oc) - sphere.radius * sphere.radius;
      float discriminant = b * b - c;
      
      if (discriminant < 0.0) {
        return hit;
      }
      
      float sqrt_discriminant = sqrt(discriminant);
      float t1 = -b - sqrt_discriminant;
      float t2 = -b + sqrt_discriminant;
      
      float dis = (t1 > 0.0) ? t1 : t2;
      
      if (dis > 0.0) {
        hit.dis = dis;
        // hit.point = ray.origin + ray.direction * dis;
        // hit.normal = normalize(hit.point - sphere.center);
        hit.color = sphere.color;
      }
      
      return hit;
    }

    HitInfo minHit(HitInfo a, HitInfo b) {
      return (a.dis < b.dis) ? a : b;
    }

    vec3 traceRay(Ray ray) {
      for(int depth = 0; depth < 10; depth++) {
        HitInfo hit;  initHit(hit);
        hit = minHit(hit, intersect(ray, sphere));
        hit = minHit(hit, intersect(ray, room));
        hit = minHit(hit, intersect(ray, portal1, portal2));
        hit = minHit(hit, intersect(ray, portal2, portal1));

        if(hit.dis >= 1e9) break;
        if(hit.portal) {
          ray = hit.ray;
        } else {
          return hit.color;
        }
      }
      
      return vec3(0.0);
    }

    void main() {
      vec2 uv = fragCoord;
      vec2 p = uv * vec2(iResolution.x / iResolution.y, 1.0);

      Ray ray;
      ray.origin = iCamera[3].xyz;
      vec3 cameraDir = normalize(vec3(p, -1.0));
      ray.direction = normalize(mat3(iCamera) * cameraDir);

      vec3 color = traceRay(ray);
      FragColor = vec4(color, 1.0);
    }
  )");

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  float quadVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  while(!window.shouldClose()) {
    window.newFrame();
    window.clearDevice(0.2f, 1.0f);
    window.clearDepth();
    window.applyCursorMode();
    window.getSize();
    window.viewPort();

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

    shader1.use();
    shader1.set("iResolution", glm::vec2(window.window.size.x, window.window.size.y));
    shader1.set("iTime", window.frame.nt);
    shader1.set("iCamera", camera.getLightTrace());
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // glBindVertexArray(0);

    window.swapBuffers();
    glfwPollEvents();
  }
	return 0;
}