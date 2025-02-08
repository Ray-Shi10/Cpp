#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>
#include <GLM.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

class Window {
public:
    GLFWwindow* window;
    unsigned int width, height;
    bool pause = false;
    bool active = true;

    bool firstMouse = true;
    glm::vec2 lastMouse = glm::vec2(0.0f);
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    template <typename T>
    class OrderManager {
        typedef std::string string;
        template <typename _Tp1>
            using vector = std::vector <_Tp1>;
        template <typename _Tp1, typename _Tp2>
            using unordered_map = std::unordered_map<_Tp1, _Tp2>;
    public:
        struct NODE {
            T* data;
            vector<NODE*> after;
            vector<NODE*> before;
        };
        vector<NODE*> nodes;
        vector<unsigned short> unused;
        unordered_map<string, NODE*> names;
        void add(T* data, string name, vector<string> after=vector<string>()) {
            NODE *node;
            if(unused.size()) {
                node = &nodes[unused.back()];
                unused.pop_back();
            } else {
                node = new NODE();
            }
            node->data = data;
            for(auto a : after) {
                if(names.find(a) == names.end()) {
                    std::cerr << "Error: " << a << " not found\n";
                    exit(-1);
                }
                node->after.push_back(names[a]);
                names[a]->before.push_back(node);
            }
            names[name] = node;
            nodes.push_back(node);
        }
    };

    Window(unsigned int width, unsigned int height, const char* title, bool active=true)
         :  width(width), height(height), active(active) {
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (window == NULL) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            exit(-1);
        } else {
            std::cout << "Created GLFW window successfully\n";
        }
        if(active) {
            glfwMakeContextCurrent(window);
        }
    }
};

#endif // _WINDOW_H_