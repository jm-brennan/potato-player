#pragma once

#include <iostream>
#include <string>
#include "gladInclude.h"
#include <glm/glm.hpp>
#include <atomic>

using namespace glm; // bad practice but its nice

enum State {
    IDLE,
    PLAYING,
    PAUSED,
    PLAYLIST_INFO
};

std::atomic<State> playerState {State::PLAYLIST_INFO};
uint secondsToSwitchToIdle = 60;
std::atomic<float> currentTrackProgress;

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

inline std::string vec_string(const vec2& vec) {
    std::string result;
    result += "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + "]";
    return result;
}

inline std::string mat_string(const mat4& mat) {
    std::string result;
    result += "[" 
                  + std::to_string(mat[0][0]) + ", "
            + " " + std::to_string(mat[0][1]) + ", "
            + " " + std::to_string(mat[0][2]) + ", "
            + " " + std::to_string(mat[0][3]) + "\n"
            + " " + std::to_string(mat[1][0]) + ", "
            + " " + std::to_string(mat[1][1]) + ", "
            + " " + std::to_string(mat[1][2]) + ", "
            + " " + std::to_string(mat[1][3]) + "\n"
            + " " + std::to_string(mat[2][0]) + ", "
            + " " + std::to_string(mat[2][1]) + ", "
            + " " + std::to_string(mat[2][2]) + ", "
            + " " + std::to_string(mat[2][3]) + "\n"
            + " " + std::to_string(mat[3][0]) + ", "
            + " " + std::to_string(mat[3][1]) + ", "
            + " " + std::to_string(mat[3][2]) + ", "
            + " " + std::to_string(mat[3][3]) + "]";
    return result;
}