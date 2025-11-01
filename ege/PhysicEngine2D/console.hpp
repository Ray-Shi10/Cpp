#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <iostream>
#include <string>

static void setColor(int r, int g, int b) { std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m"; }
static void setColor(int gray) { std::cout << "\033[38;2;" << gray << ";" << gray << ";" << gray << "m"; }
static void setColor() { std::cout << "\033[0m"; }
template <typename ...Args> inline void debug(const Args &...message) { (std::cout << ... << message); }
template <typename ...Args>
inline void error(const Args &...message) {
  setColor(200, 0, 0); debug(message...); setColor();
}
template <typename ...Args>
inline void warn(const Args &...message) {
  setColor(120, 100, 30); debug(message...); setColor();
}

#endif // CONSOLE_HPP