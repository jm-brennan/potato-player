#include "ShaderManager.h"
#include <array>
#include <algorithm>
#include "paths.h"

std::array<uint, SHADER::NUM_SHADERS> shaders;

uint compile_shader(std::string shaderFilename, GLint shaderType);
std::string load_shader_file(std::string filename);

void ShaderManager::create_shader_from_string(std::string vShaderStr, std::string fShaderStr, SHADER s) {
    uint id = glCreateProgram(); // @TODO change GLEC to be able get the value out?
    uint vid = compile_shader(vShaderStr, GL_VERTEX_SHADER);
    GLEC(glAttachShader(id, vid));
    
    uint fid = compile_shader(fShaderStr, GL_FRAGMENT_SHADER);
    GLEC(glAttachShader(id, fid));

    int success;
    GLEC(glLinkProgram(id));
    GLEC(glGetProgramiv(id, GL_LINK_STATUS, &success));
    if(!success) {
        char error[512];
        GLEC(glGetProgramInfoLog(id, 512, nullptr, error));
        printf("ERROR::Shader link:\n%s\n", error);
    }
    GLEC(glDeleteShader(vid));
    GLEC(glDeleteShader(fid));
    shaders[s] = id;
}

void ShaderManager::create_shader_from_file(std::string vShaderFile, std::string fShaderFile, SHADER s) {
    create_shader_from_string(load_shader_file(vShaderFile), load_shader_file(fShaderFile), s);
}

void ShaderManager::use(SHADER s) {
    uint id = shaders[s];
    GLEC(glUseProgram(id));
}

void ShaderManager::delete_shaders() {
    for (uint shader : shaders) {
        GLEC(glDeleteProgram(shader));
    }
}

void ShaderManager::set_float(SHADER s, const char* name, float value) {
    uint id = shaders[s];
    GLEC(glUseProgram(id));
    GLEC(glUniform1f(glGetUniformLocation(id, name), value));
}

void ShaderManager::set_vec4(SHADER s, const char* name, const vec4& vec) {
    uint id = shaders[s];
    GLEC(glUseProgram(id));
    GLEC(glUniform4fv(glGetUniformLocation(id, name), 1, value_ptr(vec)));
}

void ShaderManager::set_mat4(SHADER s, const char* name, const mat4& matrix) {
    uint id = shaders[s];
    GLEC(glUseProgram(id));
    GLEC(glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, value_ptr(matrix)));
}


uint compile_shader(std::string shaderStr, GLint shaderType) {
    uint id = glCreateShader(shaderType);
    const char* cShaderStr = shaderStr.c_str();
    GLEC(glShaderSource(id, 1, &cShaderStr, nullptr));
    GLEC(glCompileShader(id));
    int success;
    GLEC(glGetShaderiv(id, GL_COMPILE_STATUS, &success));
    if (!success) {
        char error[512];
        GLEC(glGetShaderInfoLog(id, 512, nullptr, error));
        printf("ERROR: Shader compilation failed for shader: \n%s\n\n %s\n", shaderStr.c_str(), error);
    }
    return id;
}

std::string load_shader_file(std::string filename) {
    std::ifstream shaderFile;
    std::string shaderCode;
    std::string filepath = std::string(SHADER_DIR) + filename;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        shaderFile.open(filepath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch (std::ifstream::failure e) {
        printf("ERROR::Shader file %s\n", filepath.c_str());
    }
    return shaderCode;
}



