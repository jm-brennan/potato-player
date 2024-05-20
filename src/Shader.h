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

enum SHADER {
    IMAGE,
    TEXT,
    COLOR,

    NUM_SHADERS
};


namespace shader {

void create_shader_from_string(std::string vShaderStr, std::string fShaderStr, SHADER s);
void create_shader_from_file(std::string vShaderFile, std::string fShaderFile, SHADER s);
void use(SHADER s);
uint program(SHADER s);
void delete_shaders();

void shaders_init();

}
