#include "gladInclude.h"

#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <id3v2tag.h>
#include <mpegfile.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <attachedpictureframe.h>
#include <cstdio>
#include <string.h>
#include <thread>

#include "ShaderManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "AudioFile.h"

using namespace std;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
int main(void)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    #ifdef USE_OPENGL_ES
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    #else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    //glfwGetPrimaryMonitor()
    GLFWwindow* window = glfwCreateWindow(800, 480, "Le Potato Player", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwMakeContextCurrent(window);
    
    #ifdef USE_OPENGL_ES
    gladLoadGLES2(glfwGetProcAddress);
    #else
    gladLoadGL(glfwGetProcAddress);
    #endif
    
    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera camera;
    camera.screenWidth = 800;
    camera.screenHeight = 480;
    camera.pos = vec3(0.0, 0.0, 3.0);
    camera.lookAt = vec3(0.0, 0.0, 0.0);
    camera.up = vec3(0.0, 1.0, 0.0);
    calculate_view_projection(camera);

    string vShaderTextureStr = 
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "uniform mat4 m_mvp;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = m_mvp * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
        "   v_texCoord = a_texCoord;\n"
        "}\n";
    
    string fShaderTextureStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main()\n"
        "{\n"
        "   gl_FragColor = texture2D(s_texture, v_texCoord);\n"
        "}\n";
    
    string fShaderTextStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main()\n"
        "{\n"
        "   gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(s_texture, v_texCoord).a);\n"
        "}\n";
        

    ShaderManager::create_shader_from_string(vShaderTextureStr, fShaderTextStr, SHADER::TEXT);

    ShaderManager::use(TEXT);
    GLEC(glUniform1i(glGetUniformLocation(ShaderManager::program(TEXT), "s_texture"), 0));
    std::cout << "Creating fonts\n";
    FontData smallFont = create_font("Poppins-Light.ttf", 32);
    FontData largeFont = create_font("Poppins-Medium.ttf", 64);


    ShaderManager::create_shader_from_string(vShaderTextureStr, fShaderTextureStr, SHADER::IMAGE);


    std::vector<std::string> filesToPlay = {"../tracks/I Know We'll Be Fine.mp3", "../tracks/Lamp.mp3"};

    AudioFile audio;

    std::cout << "\ngenerating text strip buffers\n";

    uint framesToSwitch = 60;
    uint frameCounter = 0;
    uint audioIndex = 0;

    const float FPS = 30.0f;
    const double FRAME_TIME = 1.0f / FPS;
    double dt = 0.0f;
    double currentTime = glfwGetTime();
    double newTime;
    double sleepTime;
    while (!glfwWindowShouldClose(window)) {
        newTime = glfwGetTime();
        dt = newTime - currentTime;
        currentTime = newTime;
        glfwPollEvents();

        if (frameCounter % framesToSwitch == 0) {
            ++audioIndex;
            audioIndex = audioIndex % filesToPlay.size();
            // TODO 
            // TODO 
            // TODO 
            // delete_data(audio)
            init(audio, filesToPlay[audioIndex]);
            generate_display_objects(audio, largeFont, smallFont);
            ShaderManager::use(IMAGE);
            GLEC(glUniform1i(glGetUniformLocation(ShaderManager::program(IMAGE), "s_texture"), 1));
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.13f, 0.14f, 0.15f, 1.0f);

        render_audio_file_display(audio, largeFont, smallFont, camera);

        glfwSwapBuffers(window);

        newTime = glfwGetTime();
        //timeTaken += (newTime - currentTime);
        //numLoops++;
        sleepTime = FRAME_TIME - (newTime - currentTime);
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds((int)(sleepTime * 1000000)));
        }
        ++frameCounter;
    }
    ShaderManager::delete_shaders();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}