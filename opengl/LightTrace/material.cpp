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

    // 定义射线结构体
    struct Ray {
      vec3 origin;    // 射线起点
      vec3 direction; // 射线方向
    };

    // 定义材质结构体
    struct Material {
      vec3 albedo;       // 基础颜色/反射率
      float roughness;   // 粗糙度 (0=镜面, 1=完全漫反射)
      float metallic;    // 金属度 (0=非金属, 1=金属)
      float reflectance; // 反射率 (0=不反射, 1=完全反射)
      vec3 emission;     // 自发光
    };

    // 定义球体结构体
    struct Sphere {
      vec3 center;    // 球心
      float radius;   // 半径
      Material material; // 材质
    };

    // 定义碰撞信息结构体
    struct HitInfo {
      float dis;          // 距离
      vec3 point;         // 碰撞点
      vec3 normal;        // 法向量
      Material material;  // 材质信息
    };

    // 定义光源结构体
    struct Light {
      vec3 position;  // 光源位置
      vec3 color;     // 光源颜色
      float intensity; // 光照强度
    };

    // 前向声明
    // vec3 traceRay(Ray ray, int depth);
    // HitInfo intersectScene(Ray ray);

    // Sphere toSphere(Light light, float radius = 0.1f) {
    //   Sphere sphere;
    //   sphere.center = light.position;
    //   sphere.radius = radius; // 光源半径
    //   sphere.color = light.color; // 光源颜色
    //   return sphere;
    // }

    // 球体交点检测函数（使用结构体）
    HitInfo intersect(Ray ray, Sphere sphere) {
      HitInfo hit;
      hit.dis = 1e9;
      
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
        hit.point = ray.origin + ray.direction * dis;
        hit.normal = normalize(hit.point - sphere.center);
        hit.material = sphere.material;
      }
      
      return hit;
    }

    // 光源作为发光球体的交点检测
    HitInfo intersect(Ray ray, Light light) {
      HitInfo hit;
      hit.dis = 1e9;

      vec3 oc = ray.origin - light.position;
      float lightRadius = 0.1; // 光源半径
      float b = dot(oc, ray.direction);
      float c = dot(oc, oc) - lightRadius * lightRadius;
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
        hit.point = ray.origin + ray.direction * dis;
        hit.normal = normalize(hit.point - light.position);
        // 光源作为自发光材质
        hit.material.albedo = vec3(0.0);
        hit.material.emission = light.color * light.intensity;
        hit.material.roughness = 0.0;
        hit.material.metallic = 0.0;
        hit.material.reflectance = 0.0;
      }

      return hit;
    }

    // Fresnel反射系数计算
    vec3 fresnelSchlick(float cosTheta, vec3 F0) {
      return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    }

    // 法线分布函数 (GGX/Trowbridge-Reitz)
    float distributionGGX(vec3 N, vec3 H, float roughness) {
      float a = roughness * roughness;
      float a2 = a * a;
      float NdotH = max(dot(N, H), 0.0);
      float NdotH2 = NdotH * NdotH;
      
      float nom = a2;
      float denom = (NdotH2 * (a2 - 1.0) + 1.0);
      denom = PI * denom * denom;
      
      return nom / denom;
    }

    // 几何遮蔽函数
    float geometrySchlickGGX(float NdotV, float roughness) {
      float r = (roughness + 1.0);
      float k = (r * r) / 8.0;
      
      float nom = NdotV;
      float denom = NdotV * (1.0 - k) + k;
      
      return nom / denom;
    }

    float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
      float NdotV = max(dot(N, V), 0.0);
      float NdotL = max(dot(N, L), 0.0);
      float ggx2 = geometrySchlickGGX(NdotV, roughness);
      float ggx1 = geometrySchlickGGX(NdotL, roughness);
      
      return ggx1 * ggx2;
    }

    // PBR光照计算
    vec3 calculatePBRLighting(HitInfo hit, Light light, vec3 viewDir) {
      if (hit.dis >= 1e9) return vec3(0.0);
      
      // 如果是自发光材质，直接返回自发光颜色
      if (length(hit.material.emission) > 0.0) {
        return hit.material.emission;
      }
      
      vec3 N = hit.normal;
      vec3 V = normalize(-viewDir);
      vec3 L = normalize(light.position - hit.point);
      vec3 H = normalize(V + L);
      
      // 计算光照距离衰减
      float distance = length(light.position - hit.point);
      float attenuation = 1.0 / (distance * distance);
      vec3 radiance = light.color * light.intensity * attenuation;
      
      // Cook-Torrance BRDF
      vec3 F0 = vec3(0.04);
      F0 = mix(F0, hit.material.albedo, hit.material.metallic);
      vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
      
      float NDF = distributionGGX(N, H, hit.material.roughness);
      float G = geometrySmith(N, V, L, hit.material.roughness);
      
      vec3 numerator = NDF * G * F;
      float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
      vec3 specular = numerator / denominator;
      
      vec3 kS = F;
      vec3 kD = vec3(1.0) - kS;
      kD *= 1.0 - hit.material.metallic;
      
      float NdotL = max(dot(N, L), 0.0);
      
      // 漫反射 + 镜面反射
      vec3 diffuse = kD * hit.material.albedo / PI;
      vec3 color = (diffuse + specular) * radiance * NdotL;
      
      // 环境光
      vec3 ambient = vec3(0.03) * hit.material.albedo;
      color += ambient;
      
      return color;
    }

    // 全局场景数据（在main函数外定义以便递归使用）
    Sphere spheres[4];
    Light lights[1];
    int numSpheres = 3;
    int numLights = 1;

    // 场景交点检测
    HitInfo intersectScene(Ray ray) {
      HitInfo closestHit;
      closestHit.dis = 1e9;
      
      // 检测所有球体
      for(int i = 0; i < numSpheres; i++) {
        HitInfo hit = intersect(ray, spheres[i]);
        if(hit.dis < closestHit.dis) {
          closestHit = hit;
        }
      }
      
      // 检测所有光源
      for(int i = 0; i < numLights; i++) {
        HitInfo hit = intersect(ray, lights[i]);
        if(hit.dis < closestHit.dis) {
          closestHit = hit;
        }
      }
      
      return closestHit;
    }
    // 迭代光线追踪（替换递归版本）
    vec3 traceRay(Ray ray) {
      vec3 finalColor = vec3(0.0);
      vec3 throughput = vec3(1.0); // 光线能量衰减因子

      for(int depth = 0; depth < MAX_DEPTH; depth++) {
        HitInfo hit = intersectScene(ray);
        
        if(hit.dis >= 1e9) {
          // 天空盒颜色
          finalColor += throughput * vec3(0.02, 0.02, 0.05);
          break;
        }
        
        // 如果有自发光，添加到最终颜色
        if(length(hit.material.emission) > 0.0) {
          finalColor += throughput * hit.material.emission;
          break;
        }
        
        // 计算直接光照
        vec3 directLighting = vec3(0.0);
        for(int i = 0; i < numLights; i++) {
          directLighting += calculatePBRLighting(hit, lights[i], ray.direction);
        }
        
        // 添加直接光照到最终颜色
        finalColor += throughput * directLighting;
        
        // 检查是否需要反射
        if(hit.material.reflectance <= 0.0 || depth >= MAX_DEPTH - 1) {
          break;
        }
        
        // 计算反射能量衰减
        float fresnel = pow(1.0 - max(dot(-ray.direction, hit.normal), 0.0), 5.0);
        float reflectStrength = mix(hit.material.reflectance, 1.0, fresnel);
        reflectStrength *= (1.0 - hit.material.roughness);
        
        // 更新能量衰减因子
        throughput *= reflectStrength;
        
        // 如果能量太小，停止追踪
        if(max(max(throughput.x, throughput.y), throughput.z) < 0.01) {
          break;
        }
        
        // 准备下一条射线（反射射线）
        vec3 reflectDir = reflect(ray.direction, hit.normal);
        ray.origin = hit.point + hit.normal * 0.001; // 避免自相交
        ray.direction = reflectDir;
      }
      
      return finalColor;
    }

    HitInfo minHit(HitInfo a, HitInfo b) {
      return (a.dis < b.dis) ? a : b;
    }

    void main() {
      vec2 uv = fragCoord;
      vec2 p = uv * vec2(iResolution.x / iResolution.y, 1.0);

      // 创建射线
      Ray ray;
      ray.origin = iCamera[3].xyz;
      vec3 cameraDir = normalize(vec3(p, -1.0));
      ray.direction = normalize(mat3(iCamera) * cameraDir);

      // 初始化场景数据
      // 材质1：红色漫反射材质，带轻微反射
      spheres[0].center = vec3(0.0, 0.0, 0.0);
      spheres[0].radius = 0.5;
      spheres[0].material.albedo = vec3(0.8, 0.2, 0.2);
      spheres[0].material.roughness = 0.3;
      spheres[0].material.metallic = 0.0;
      spheres[0].material.reflectance = 0.2;
      spheres[0].material.emission = vec3(0.0);

      // 材质2：金色金属材质，高反射率
      spheres[1].center = vec3(1.5, 0.0, 0.0);
      spheres[1].radius = 0.4;
      spheres[1].material.albedo = vec3(1.0, 0.86, 0.57);
      spheres[1].material.roughness = 0.05;
      spheres[1].material.metallic = 1.0;
      spheres[1].material.reflectance = 0.9;
      spheres[1].material.emission = vec3(0.2);

      // 材质3：镜面材质，完全反射
      spheres[2].center = vec3(-1.2, -0.3, 0.5);
      spheres[2].radius = 0.3;
      spheres[2].material.albedo = vec3(0.95, 0.95, 0.95);
      spheres[2].material.roughness = 0.0;
      spheres[2].material.metallic = 1.0;
      spheres[2].material.reflectance = 1.0;
      spheres[2].material.emission = vec3(0.1);

      // 光源
      lights[0].position = vec3(2.0, 2.0, -1.0);
      lights[0].color = vec3(1.0, 1.0, 1.0);
      lights[0].intensity = 50.0;

      // 使用光线追踪
      vec3 color = traceRay(ray);
      
      // 色调映射和伽马校正
      color = color / (color + vec3(1.0));
      color = pow(color, vec3(1.0/2.2));

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