#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <iostream>
#include <fstream>

namespace tools {
    enum class Color : unsigned int {
        BLACK        = 0xff000000,
        RED          = 0xffff0000,
        GREEN        = 0xff00ff00,
        BLUE         = 0xff0000ff,
        YELLOW       = 0xffffff00,
        MAGENTA      = 0xffff00ff,
        CYAN         = 0xff00ffff,
        WHITE        = 0xffffffff,
        LIGHT_GRAY   = 0xffc0c0c0,
        GRAY         = 0xff808080,
        DARK_GRAY    = 0xff303030,
    };
    namespace console {
        template <typename OStream, typename... Args>
        void print(OStream stream, Args&&... args) {
            stream.start();
            (stream << ... << std::forward<Args>(args));
            stream.flush();
        }
        namespace streams {
            class OStream {
                std::ofstream *fileStream = nullptr;
                Color color = Color::LIGHT_GRAY;
            public:
                void start() {}
                template <typename T>
                OStream& operator<<(const T& value) {
                    ;
                    return *this;
                }
                void flush() {
                    // Flush the stream
                }
            };
        }
    }
    namespace ____tmp {
        class __init_console_color {
        public:
        }
    }
}

#endif // _CONSOLE_H_