#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "gladInclude.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "utils/paths.h"
#include "definitions.h"

using namespace glm;

enum SHADER {
    COLOR,
    //IMAGE,
    //TEXT,

    NUM_SHADERS
};

struct ColorShaderInfo {
    uint mvpLoc = 0;
};

class ShaderManager {
public:
    static void create_shader_from_string(std::string vShaderStr, std::string fShaderStr, SHADER s);
    static void create_shader_from_file(std::string vShaderFile, std::string fShaderFile, SHADER s);
    static void use(SHADER s);

    static void set_float(SHADER s, const char* name, float value);
    static void set_vec4 (SHADER s, const char* name, const vec4& vec);
    static void set_mat4 (SHADER s, const char* name, const mat4& matrix);

    static void delete_shaders();
};