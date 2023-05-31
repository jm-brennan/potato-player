#define GLAD_GLES2_IMPLEMENTATION
#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace glm;
using namespace std;

static void glfw_error_callback(int error, const char* description)
{
    cerr << "GLFW Error: " << description << "\n";
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        cerr << "Could not initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL ES 2.0 Triangle (EGL)", NULL, NULL);

    if (!window) {
        // apparently if window creation fails, it may be worthwhile to retry after
        // setting glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

        glfwTerminate();
        cerr << "Could not create window\n";
        return 1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLES2(glfwGetProcAddress);
    glfwSwapInterval(1);

}