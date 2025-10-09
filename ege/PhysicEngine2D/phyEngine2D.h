#ifndef ALL_HEADERS_H
#define ALL_HEADERS_H

constexpr float ESP = 1e-6;
constexpr float PI = 3.14159265358979323846f;
float rand01() { return static_cast<float>(std::rand()) / RAND_MAX; }
template <typename T>
std::string toStr(const T& val) { return val.toStr(); }
std::string toStr(float val) { return std::to_string(val); }
std::string toStr(int val) { return std::to_string(val); }
std::string toStr(bool val) { return val ? "true" : "false"; }

#include "color.hpp"
#include "vec2.hpp"
#include "shape.hpp"
#include "collision.hpp"
#include "obj.hpp"
#include "window.hpp"
#include "world.hpp"

#endif // ALL_HEADERS_H