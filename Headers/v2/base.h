#ifndef _BASE_H_
#define _BASE_H_

#include "GLFW.h"
#include "GLM.h"
#include "Colorful.h"
#include "Error.h"
//#include "TypeTraits.h"

void initConsole() { HANDLE stdHandle; DWORD dwMode;
    stdHandle = GetStdHandle(STD_INPUT_HANDLE ); GetConsoleMode(stdHandle, &dwMode); dwMode |= 0x0200; SetConsoleMode(stdHandle, dwMode);
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE); GetConsoleMode(stdHandle, &dwMode); dwMode |= 0x0004; SetConsoleMode(stdHandle, dwMode);
}
void initGLAD() {
    std::out << "Initializing GLAD\r";
    if(!/**gladLoadGL()/*/gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)/**/) {
        std::error << "Failed to initialize GLAD\n"; exit(-1);
    } else {
        std::succ << "Initialized GLAD successfully\n";
    }
}

#endif // _BASE_H_