#ifndef COLOR_HPP
#define COLOR_HPP

#include <random>

class Color {
  using byte = unsigned char;
  byte clamp(float val) const {
    if(val < 0) return 0;
    if(val > 255) return 255;
    return static_cast<byte>(val);
  }
 public:
  byte r, g, b;
  Color() : r(0), g(0), b(0) {}
  Color(byte gray) : r(gray), g(gray), b(gray) {}
  Color(int gray) : Color(clamp(gray)) {}
  Color(unsigned long rgb) :
    r((rgb>>16)&0xFF), g((rgb>>8)&0xFF), b(rgb&0xFF) {}
  Color(byte r, byte g, byte b) : r(r), g(g), b(b) {}
  Color(int r, int g, int b) : r(clamp(r)), g(clamp(g)), b(clamp(b)) {}
  Color(float r, float g, float b) : r(clamp(r)), g(clamp(g)), b(clamp(b)) {}
  operator unsigned long() const { return 0xff000000 | (r<<16) | (g<<8) | b; }
  Color operator*(float scalar) const { return Color(r*scalar, g*scalar, b*scalar); }
  Color operator+(const Color& c) const { return Color(r+c.r, g+c.g, b+c.b); }
  static Color rand() { return Color(std::rand()%256, std::rand()%256, std::rand()%256); }
  static Color rand(int min, int max) {
    if(min < 0) min = 0;
    if(max > 255) max = 255;
    if(min > max) std::swap(min, max);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return Color(dis(gen), dis(gen), dis(gen));
  }
};

#endif // COLOR_HPP