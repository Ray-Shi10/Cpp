#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <iostream>
#include <windows.h>

class Console {
public:
    int left, top, width, height;
    HANDLE hOut, hIn;
    wchar_t *buffer;
    Console(int left, int top, int width, int height) : 
        left(left), top(top), width(width), height(height), 
        hOut(GetStdHandle(STD_OUTPUT_HANDLE)), hIn(GetStdHandle(STD_INPUT_HANDLE)) {
        buffer = new wchar_t[width*height];
        for(int i=0; i<height; i++) {
            for(int j=0; j<width; j++) {
                charAt(i,j) = L' ';
            }
        }
    }
    ~Console() { delete[] buffer; }
    wchar_t& charAt(int i, int j) { return buffer[i*width+j]; }
    wchar_t const& charAt(int i, int j) const { return buffer[i*width+j]; }
    wchar_t* operator[](int i) { return buffer + i*width; }
    wchar_t const* operator[](int i) const { return buffer + i*width; }
    void setCorcurPos(int x, int y) {
        COORD coord({x, y});
        SetConsoleCursorPosition(hOut, coord);
    }
    void getCorcurPos(int &x, int &y) const {
        POINT pos;
        GetCursorPos(&pos);
        //ScreenToClient(hOut, &pos);
        x = pos.x; y = pos.y;
    }
    void clear() {/*
        for(int i=0; i<height; i++) {
            for(int j=0; j<width; j++) {
                printf(" ");
            }
            printf("\n");
        }*/
    }
};

#endif // _CONSOLE_H_