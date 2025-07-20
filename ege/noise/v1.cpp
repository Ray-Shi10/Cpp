#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <functional>
#include <ctime>
#include <ege.h>

constexpr float PI = 3.14159265358979323846f;

template <typename T>
class Vector2D {
public:
    T x, y;
    Vector2D(T x = 0, T y = 0) : x(x), y(y) {}
    Vector2D(const Vector2D& other) : x(other.x), y(other.y) {}
    Vector2D& operator=(T value) { x = value; y = value; return *this; }
    Vector2D& operator=(const Vector2D& other) { x = other.x; y = other.y; return *this; }
    Vector2D operator+(const Vector2D& other) const { return Vector2D(x + other.x, y + other.y); }
    Vector2D operator-(const Vector2D& other) const { return Vector2D(x - other.x, y - other.y); }
    Vector2D operator*(T scalar) const { return Vector2D(x * scalar, y * scalar); }
    Vector2D operator/(T scalar) const { return Vector2D(x / scalar, y / scalar); }
    Vector2D operator-() const { return Vector2D(-x, -y); }
    Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
    Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
    Vector2D& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2D& operator/=(T scalar) { x /= scalar; y /= scalar; return *this; }
    bool operator==(const Vector2D& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2D& other) const { return !(*this == other); }
    T length() const { return std::sqrt(x * x + y * y); }
    T lengthSquared() const { return x * x + y * y; }
    Vector2D normal() const { T len = length(); return Vector2D(x / len, y / len); }
    Vector2D perpendicular() const { return Vector2D(-y, x); }
    Vector2D &normalize() { T len = length(); x /= len; y /= len; return *this; }
    T dot(const Vector2D& other) const { return x * other.x + y * other.y; }
};
using Vec2f = Vector2D<float>;
using Vec2i = Vector2D<int>;

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}
float lerp(float a, float b, float t) {
    return a + (b - a) * fade(t);
}
float lerp(float a, float b, float t1, float t2) {
    return lerp(a, b, t1/(t1 + t2));
}
ege::color_t gray(unsigned char c) {
    return EGERGB(c, c, c);
}

int main() {
    constexpr int width = 40, height = 30, size = 20;
    ege::initgraph(width * size, height * size);
    //Vec2f grad[width+1][height+1];
    float noise[width+1][height+1];
    std::srand(std::time(0));
    __start:
    for(int i = 0; i <= width; ++i) {
        for(int j = 0; j <= height; ++j) {
            //float angle = static_cast<float>(std::rand()) / RAND_MAX * 2 * PI;
            //grad[i][j] = Vec2f(std::cos(angle), std::sin(angle));
            noise[i][j] = static_cast<float>(std::rand()) / RAND_MAX;
        }
    }
    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            for(int x = 0; x < size; ++x) {
                for (int y = 0; y < size; ++y) {
                    Vec2f p = Vec2f(x, y) / size;
                    //float g0 = grad[i][j], g1 = grad[i+1][j], g2 = grad[i][j+1], g3 = grad[i+1][j+1];
                    float n03 = lerp(noise[i][j], noise[i][j+1], p.y);
                    float n12 = lerp(noise[i+1][j], noise[i+1][j+1], p.y);
                    float n = lerp(n03, n12, p.x);
                    short color(n*256);
                    if(color < 0) color = 0;
                    if(color > 255) color = 255;/**/
                    ege::putpixel(i*size+x, j*size+y, gray(color));/*/
                    static constexpr int range = 12;
                    if(128-range <= color && color <= 128+range) {
                        ege::putpixel(i*size+x, j*size+y, gray(color));
                    }/**/
                }
            }
        }
    }
    ege::getch();
    goto __start;
    return 0;
}
