#ifndef ALL_HEADERS_H
#define ALL_HEADERS_H

const float ESP = 1e-6;
float rand01() { return static_cast<float>(std::rand()) / RAND_MAX; }

#include "color.hpp"
#include "vec2.hpp"
#include "window.hpp"
#include "obj.hpp"
#include "world.hpp"

#endif // ALL_HEADERS_H