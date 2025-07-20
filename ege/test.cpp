#include <ege.h>
#include <iostream>
using std::cout;
#include <fstream>
#include <string>
using std::string;
#include <list>
using std::list;
#include <cmath>

struct point_t { int x=0, y=0; };

string readFile(const string& filePath) {
    std::ifstream file(filePath);
    if(!file.is_open()) throw std::runtime_error("Could not open file");
    string content{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    }; file.close(); return content;
}

using Lines = list<string>;
Lines splitLines(const string& text) {
    Lines lines;
    string line;
    for(char c : text) {
        switch(c) {
            case '\n':
                lines.push_back(line);
                line.clear();
                break;
            case '\r':
                break;
            case '\t':
                line += string(4-line.length()%4, ' ');
                break;
            default:
                line += c;
                break;
        }
    } lines.push_back(line);
    return lines;
}

int printLines(point_t pos, const Lines& lines) {
    int maxWidth = 0;
    for(const auto& line : lines) {
        ege::outtextxy(pos.x, pos.y, line.c_str());
        int width = ege::textwidth(line.c_str());
        if(width > maxWidth) maxWidth = width;
        pos.y += ege::textheight(line.c_str());
    } return maxWidth;
}

int main(int argc, char* argv[]) {
    for(int i=0; i<argc; i++) cout << argv[i] << "\n";
    ege::initgraph(640, 480); ege::cleardevice();
    ege::setcaption("Text Editor");
    ege::setcodepage(EGE_CODEPAGE_UTF8);
    ege::setcolor(ege::COLORS::WHITE);
    constexpr int fontHeight = 20, fontWidth = 10;
    ege::setfont(fontHeight, fontWidth, "Consolas");
    point_t canvasPos;
    bool isShift = false;
    string text = readFile(argv[1]);
    Lines lines = splitLines(text);
    int height = ege::textheight(lines.front().c_str()) * (lines.size()-1) - 10;
    for(; ege::is_run(); ege::delay_fps(60)) {
        ege::cleardevice();
        int width = printLines(canvasPos, lines) - fontWidth;
        while(ege::kbmsg()) {
            ege::key_msg msg = ege::getkey();
            switch(msg.key) {
                case ege::key_shift:
                    isShift = msg.msg==ege::key_msg_down; break;
                case ege::key_esc:
                    return 0;
            }
        }
        while(ege::mousemsg()) {
            ege::mouse_msg msg = ege::getmouse();
            if(msg.is_wheel()) {
                int *coord = isShift ? &canvasPos.x : &canvasPos.y;
                *coord += msg.wheel * 0.8;
                if(*coord > 0) *coord = 0;
                int coordMax = isShift ? width : height;
                if(*coord < -coordMax) *coord = -coordMax;
            } else if(msg.is_down());
        }
    }
    ege::closegraph();
    return 0;
}
