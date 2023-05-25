#pragma once

#include <iostream>
#include <glad/glad.h>
//#include <GL/openglut.h>

#define GLEC(x) gl_clear_error();\
    x;\
    gl_log_call(#x, __FILE__, __LINE__)

static void gl_clear_error() {
    while (glGetError() != GL_NO_ERROR);
}

static bool gl_log_call(const char* function, const char* file, int line) {
    bool error = false;
    if (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ") in file " << file 
                << " in function " << function << " on line " << line << std::endl;
        error = true;
    }
    return error;
}

static const float PI = 3.14159f;
static const float PI_HALF = 1.570795;
static const float PI_2 = 6.28318f;