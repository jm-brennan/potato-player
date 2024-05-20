#include "Shader.h"
#include <array>
#include <algorithm>
#include "paths.h"

namespace shader {

std::array<uint, SHADER::NUM_SHADERS> shaders;

uint compile_shader(std::string shaderFilename, GLint shaderType);
std::string load_shader_file(std::string filename);

void create_shader_from_string(std::string vShaderStr, std::string fShaderStr, SHADER s) {
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

void create_shader_from_file(std::string vShaderFile, std::string fShaderFile, SHADER s) {
    create_shader_from_string(load_shader_file(vShaderFile), load_shader_file(fShaderFile), s);
}

void use(SHADER s) {
    GLEC(glUseProgram(shaders[s]));
}

uint program(SHADER s) {
    return shaders[s];
}

void delete_shaders() {
    for (uint shader : shaders) {
        GLEC(glDeleteProgram(shader));
    }
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

void shaders_init() {
    std::string vShaderTextureStr = 
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "uniform mat4 m_mvp;\n"
        "void main() {\n"
        "   gl_Position = m_mvp * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
        "   v_texCoord = a_texCoord;\n"
        "}\n";
    
    std::string fShaderTextureStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main() {\n"
        "   gl_FragColor = texture2D(s_texture, v_texCoord);\n"
        "}\n";
    
    std::string fShaderTextStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(s_texture, v_texCoord).a);\n"
        "}\n";
    
    std::string fShaderColorStr =
        "precision mediump float;\n"
        "//uniform vec4 v_color;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n";


    shader::create_shader_from_string(vShaderTextureStr, fShaderTextStr, SHADER::TEXT);
    shader::create_shader_from_string(vShaderTextureStr, fShaderColorStr, SHADER::COLOR);

    shader::create_shader_from_string(vShaderTextureStr, fShaderTextureStr, SHADER::IMAGE);
}


}
