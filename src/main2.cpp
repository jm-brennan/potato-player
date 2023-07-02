#include "gladInclude.h"

#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderManager.h"
//#include "Camera.h"

using namespace glm;
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

    GLFWwindow* window = glfwCreateWindow(640, 480, "window", nullptr, nullptr);
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

    string vShaderStr = 
        "attribute vec4 vPosition;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vPosition;\n"
        "}\n";

    string fShaderStr =
        "precision mediump float;\n"
        "void main()\n"
        "{\n"
        "   gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);\n"
        "}\n";

    ShaderManager::create_shader_from_string(vShaderStr, fShaderStr, SHADER::COLOR);

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        

        glClearColor(0.19f, 0.65f, 0.32f, 1.0f);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
