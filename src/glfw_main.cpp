#include <cstdlib>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "definitions.h"

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void center_window(GLFWwindow* window);
void main_loop(GLFWwindow* window);

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s (%d)\n", description, error);
}

unsigned int SCREEN_WIDTH = 1920;//1140
unsigned int SCREEN_HEIGHT = 1080;//641
// globals for the glfw callbacks to access

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(error_callback);

    // Glfw leaks 48,157 bytes
    if (!glfwInit()) {
        printf("ERROR::GLFW init\n");
        return -1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // @TODO call with glfwGetPrimaryMonitor() to do fullscreen, use cl args?
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Potato Player", nullptr, nullptr);
    if (!window) {
        printf("ERROR::GLFW create window. May have invalid GPU drivers.\n");
        glfwTerminate();
        return -1;
    }

    center_window(window);
    glfwMakeContextCurrent(window);

    //glfwSetKeyCallback(window, key_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSwapInterval(1);

    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR::GLAD init\n");
        return -1;
    }

    GLEC(glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
    GLEC(glEnable(GL_BLEND));
    GLEC(glDisable(GL_DEPTH_TEST));
    GLEC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    main_loop(window);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void main_loop(GLFWwindow* window) {
    const float FPS = 30.0f;
    const double FRAME_TIME = 1.0f / FPS;
    double dt = 0.0f;
    double currentTime = glfwGetTime();
    double newTime;
    double sleepTime;

    // for timing tests
    //double timeTaken = 0.0;
    //int numLoops = 0;

    while (!glfwWindowShouldClose(window)) {
        newTime = glfwGetTime();
        dt = newTime - currentTime;
        currentTime = newTime;

        //im->update_mouse_pos();
        //glfwPollEvents(); // inputmanager receives newKeyDows and newKeyUps

        //game1.process_input((float)dt);
        //game1.update((float)dt);
        //im->update_new_inputs(); // resolve newKeyDowns to keyDowns, same for mouse

        GLEC(glClearColor(0.19f, 0.65f, 0.32f, 1.0f));
        GLEC(glClear(GL_COLOR_BUFFER_BIT));
        //game1.render();

        GLEC(glfwSwapBuffers(window));

        newTime = glfwGetTime();
        //timeTaken += (newTime - currentTime);
        //numLoops++;
        sleepTime = FRAME_TIME - (newTime - currentTime);
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds((int)(sleepTime * 1000000)));
        }
    }
    //printf("%f seconds for %d loops\n", timeTaken, numLoops);
}
/* 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            im->set_key_status(key, true);
        } else if (action == GLFW_RELEASE) {
            im->set_key_status(key, false);
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        im->set_mouse_status(true);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        im->set_mouse_status(false);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    GLEC(glViewport(0, 0, width, height));
    game->set_window_size((unsigned int)width, (unsigned int)height);
} */

void center_window(GLFWwindow *window) {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    if (!monitor) return;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode) return;

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glfwSetWindowPos(window,
                     monitorX + (mode->width - windowWidth) / 2,
                     monitorY + (mode->height - windowHeight) / 2);
}
