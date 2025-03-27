#ifndef _COLORFUL_H_
#define _COLORFUL_H_

#include <iostream>

namespace std {
    static void setColor(ostream &stream, int r, int g, int b) { stream << "\033[38;2;" << r << ";" << g << ";" << b << "m"; }
    static void setColor(ostream &stream, int gray) { stream << "\033[38;2;" << gray << ";" << gray << ";" << gray << "m"; }
    static void setColor(ostream &stream) { stream << "\033[0m"; }
    class color_ostream {
        void init() { sprintf(setColor, "\033[38;2;%03d;%03d;%03dm", r, g, b); }
        template <typename T> void print(T value) { stream << setColor << value << "\033[0m"; }
    public:
        using byte = unsigned char; byte r, g, b; ostream &stream; char setColor[32];
        color_ostream(ostream &stream, byte r, byte g, byte b) : stream(stream), r(r), g(g), b(b) { init(); }
        color_ostream(ostream &stream, byte gray) : color_ostream(stream, gray, gray, gray) { init(); }
        template <typename T> color_ostream &operator<<(T value) { print(value); return *this; }
        color_ostream operator<<(color_ostream const&stream) { return color_ostream(this->stream, stream.r, stream.g, stream.b); }
        color_ostream operator<<(ostream &stream) { return color_ostream(stream, r, g, b); }
        color_ostream &operator<<(ostream&(*func)(ostream&)) { stream << setColor << func << "\033[0m"; return *this; }
        operator ostream&() { return stream; } operator ostream const&() const { return stream; } operator bool() { return true; }
        //template <typename T> using _print_func = void(*)(T); template <typename T> operator _print_func<T>() { return [](T value){(*this)<<vaule;}; }
    } error(cerr, 200,0,0), warn(cerr, 200,64,32), succ(cout, 32,200,32), out(cout, 200);
}

#endif // _COLORFUL_H_