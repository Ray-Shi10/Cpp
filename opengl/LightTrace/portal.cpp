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

    struct Portal {
      vec3 center;     // 传送门中心
      // vec3 normal;     // 传送门法向量
      float radius;    // 传送门半径
      vec3 color;      // 传送门颜色
      vec3 up,right;
    };

    struct Room {
      vec3 minBounds;  // 最小边界
      vec3 maxBounds;  // 最大边界
      vec3 color;      // 房间颜色
    };

    struct HitInfo {
      float dis;          // 距离
      // vec3 point;         // 碰撞点
      // vec3 normal;        // 法向量
      vec3 color;         // 颜色
      bool portal;
      Ray ray;            // 射线
    };
    void initHit(out HitInfo hit) {
      hit.dis = 1e9;
      hit.color = vec3(0.0);
      hit.portal = false;
      hit.ray.origin = vec3(0.0);
      hit.ray.direction = vec3(0.0, 0.0, 0.0);
    }

    // 球体交点检测函数（使用结构体）
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
      
      // 选择最近的正交点
      float dis = (t1 > 0.0) ? t1 : t2;
      
      if (dis > 0.0) {
        hit.dis = dis;
        // hit.point = ray.origin + ray.direction * dis;
        // hit.normal = normalize(hit.point - sphere.center);
        hit.color = sphere.color;
      }
      
      return hit;
    }

    HitInfo intersect(Ray ray, Portal portal, Portal target) {
      HitInfo hit;  initHit(hit);
      
      // 计算射线与传送门平面的交点
      vec3 normal = cross(portal.up, portal.right);
      float denom = dot(normal, ray.direction);
      if (abs(denom) < 0.0001) return hit; // 射线与平面平行
      
      vec3 p0l0 = portal.center - ray.origin;
      float t = dot(p0l0, normal) / denom;

      if (t < 0.0) return hit; // 交点在射线起点后面
      
      vec3 intersection = ray.origin + ray.direction * t;
      vec3 offset = intersection - portal.center;
      float distance = length(offset);
      
      if (distance <= portal.radius-0.1f) {
        hit.dis = t;
        // hit.point = intersection;
        // hit.normal = portal.normal;
        // hit.color = portal.color;
        hit.portal = true;
        vec2 origin = vec2(dot(offset, portal.right), dot(offset, portal.up));
        vec3 direction = vec3(dot(ray.direction, portal.right), dot(ray.direction, portal.up), dot(ray.direction, normal));
        hit.ray.origin = target.center + origin.x * target.right + origin.y * target.up;
        hit.ray.direction = normalize(direction.x * target.right + direction.y * target.up + direction.z * cross(target.right, target.up));
        hit.ray.origin += hit.ray.direction * 0.001;
      } else if (distance <= portal.radius) {
        hit.dis = t;
        // hit.point = intersection;
        // hit.normal = portal.normal;
        hit.color = portal.color;
      }

      return hit;
    }

    bool grid(vec3 p, float size) {
      vec3 coord = p / size;
      vec3 grid = -abs(fract(coord)-0.5f);
      
      float line1 = step(-0.05, grid.x);
      float line2 = step(-0.05, grid.y);
      float line3 = step(-0.05, grid.z);

      return line1 + line2 + line3 > 0.0f;
    }
    // 房间（立方体）交点检测函数
    HitInfo intersect(Ray ray, Room room) {
      HitInfo hit;  initHit(hit);
      
      // 计算与六个面的交点
      vec3 invDir = 1.0 / ray.direction;
      vec3 t1 = (room.minBounds - ray.origin) * invDir;
      vec3 t2 = (room.maxBounds - ray.origin) * invDir;
      
      vec3 tmin = min(t1, t2);
      vec3 tmax = max(t1, t2);
      
      float tnear = max(max(tmin.x, tmin.y), tmin.z);
      float tfar = min(min(tmax.x, tmax.y), tmax.z);
      
      if (tnear > tfar || tfar < 0.0) return hit;
      
      float t = (tnear > 0.0) ? tnear : tfar;
      if (t > 0.0) {
        hit.dis = t;
        // hit.point = ray.origin + ray.direction * t;
        
        // 计算法向量
        vec3 center = (room.minBounds + room.maxBounds) * 0.5;
        // vec3 p = hit.point - center;
        vec3 p = (ray.origin + ray.direction * t) - center;
        vec3 d = (room.maxBounds - room.minBounds) * 0.5;
        
        if (abs(p.x) > abs(p.y) && abs(p.x) > abs(p.z)) {
          // hit.normal = vec3(sign(p.x), 0, 0);
        } else if (abs(p.y) > abs(p.z)) {
          // hit.normal = vec3(0, sign(p.y), 0);
        } else {
          // hit.normal = vec3(0, 0, sign(p.z));
        }
        
        // 添加网格效果
        bool gridPattern = grid((ray.origin + ray.direction * t), 1.0f);
        hit.color = gridPattern ? vec3(0) : room.color;
      }
      
      return hit;
    }

    HitInfo minHit(HitInfo a, HitInfo b) {
      return (a.dis < b.dis) ? a : b;
    }

    // 传送门射线追踪函数（迭代版本）
    vec3 traceRay(Ray ray) {
      // Ray ray = initialRay;
      // vec3 finalColor = vec3(0.0);
      // float colorWeight = 1.0;

      Sphere sphere;
      sphere.center = vec3(2.0, 0.0, -3.0);
      sphere.radius = 0.8;
      sphere.color = vec3(1.0, 0.0, 0.0);
      
      Room room;
      room.maxBounds = vec3(18, 6, 18);
      room.minBounds = -room.maxBounds;
      room.color = vec3(0.9);

      Portal portal1;
      portal1.center = vec3(-2.0, 0.0, -4.0);
      // portal1.normal = vec3(0.0, 0.0, 1.0);
      portal1.up    = normalize(vec3(0.0, 1.0, 0.0));
      portal1.right = normalize(vec3(1.0, 0.0, 0.0));
      portal1.radius = 1.0;
      portal1.color = vec3(0.0, 0.5, 1.0); // 蓝色传送门
      
      Portal portal2;
      portal2.center = vec3(2.0, 0.0, -6.0);
      // portal2.normal = vec3(0.0, 0.0, -1.0);
      portal2.up    = normalize(vec3(0.0, 1.0, 0.0));
      portal2.right = normalize(vec3(1.0, 0.0, 0.0));
      portal2.radius = 1.0;
      portal2.color = vec3(1.0, 0.5, 0.0); // 橙色传送门
      
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