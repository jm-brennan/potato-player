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
#include <mutex>
#include <filesystem>

#include "ShaderManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "miniaudio.h"

#include "Playlist.h"
#include "AudioFile.h"
#include "audio_player.h"

using namespace std;

const uint NUM_PLAYLISTS = 6;

enum State {
    IDLE,
    PLAYING,
    PAUSED,
    PLAYLIST_INFO,
    SHUTDOWN
};

std::string StateString(State state) {
    switch (state) {
        case IDLE:
            return "IDLE";
        case PLAYING:
            return "PLAYING";
        case PAUSED:
            return "PAUSED";
        case PLAYLIST_INFO:
            return "PLAYLIST_INFO";
        case SHUTDOWN:
            return "SHUTDOWN";
        default:
            return "INVALID STATE";
    }
}

void blank_screen() {
    std::cout << "disable screen\n";
}

void render_playlists_infO(const std::map<uint, Text>& playlists, const FontData& font, Camera& camera) {
    for (const std::pair<uint, Text>& playlist : playlists) {
        render_text(playlist.second, font, camera);
    }
}

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


void play(State& playerState, ma_device& device) {
    if (playerState != State::PLAYING) {
        if (ma_device_start(&device) != MA_SUCCESS) {
            printf("Failed to start playback device, shutting down decoders\n");
            uninitialize_device(device);
            uninitialize_decoders();
            return;
        }
        else {
            playerState = PLAYING;
            std::cout << "Playing\n";
        }
    }
    else {
        std::cout << "Already playing\n";
    } 
}

void pause(State& playerState, ma_device& device) {
    if (playerState != State::PAUSED) {
        if (ma_device_stop(&device) != MA_SUCCESS) {
            printf("Failed to stop playback device, shutting down decoders\n");
            uninitialize_device(device);
                uninitialize_decoders();
            return;
        }
        else {
            playerState = PAUSED;
            std::cout << "Paused\n";
        }
    }
    else {
        std::cout << "Already paused\n";
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

    State playerState = PLAYLIST_INFO;

    Playlists playlists = parse_playlists();

    ShaderManager::use(TEXT);
    std::cout << "Creating playlists display\n";
    std::map<uint, Text> playlistsDisplay;
    for (const std::pair<uint, Playlist>& playlist : playlists) {
        uint playlistID = playlist.first;
        if (playlistID > 0 && playlistID <= NUM_PLAYLISTS) {
            init(playlistsDisplay[playlistID], playlist.second.name);
            layout_text(playlistsDisplay[playlistID], smallFont);
            generate_text_strip_buffers(playlistsDisplay[playlistID].textStrip);
            std::cout << "adding playlist text for playlist " << playlist.second.name
                      << " to playlist slot " << playlistID << "\n";
        }
        else {
            std::cout << "Playlist \"" << playlist.second.name << "\" is outside of playable range found, not including\n";
        }
    }

    vec2 playlistDisplayPos = vec2(40, 440);
    for (std::pair<const uint, Text>& playlist : playlistsDisplay) {
        playlist.second.model.pos = playlistDisplayPos;
        std::cout << "set position for playlist " << playlist.second.str << " to " << vec_string(playlist.second.model.pos) << "\n";
        playlistDisplayPos.y -= 40;
    }


    std::random_device random;


    std::vector<std::filesystem::path> playlist = randomize_playlist(playlists[1], random);

    std::cout << "playlist size " << playlist.size() << ":\n";
    for (auto p : playlist) {
        std::cout << p << "\n";
    }

    std::vector<std::pair<bool, AudioFile>> fileDisplays;
    fileDisplays.resize(playlist.size());
    uint currentPathsIndex = 0;
    init(fileDisplays[currentPathsIndex].second, playlist[currentPathsIndex]);
    generate_display_objects(fileDisplays[currentPathsIndex].second, largeFont, smallFont);
    fileDisplays[currentPathsIndex].first = true;
    ShaderManager::use(IMAGE);
    GLEC(glUniform1i(glGetUniformLocation(ShaderManager::program(IMAGE), "s_texture"), 1));

    ma_device_config deviceConfig;
    ma_device device;

    init_decoder_config();

    bool firstPlay = true;

    uint framesToSwitch = 210;
    uint frameCounter = 0;

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

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.13f, 0.14f, 0.15f, 1.0f);

        // process input
        State newState = (State)((uint)(frameCounter / framesToSwitch) % 4);
        std::cout << "current state is " << StateString(playerState)
                  << ", state to switch to is " << StateString(newState) << "\n";

        uint newPathsIndex = pathsIndex.load();
        if (currentPathsIndex != newPathsIndex && !fileDisplays[newPathsIndex].first) {
            std::cout << "generating display objects for " << paths[newPathsIndex] << "\n";
            init(fileDisplays[newPathsIndex].second, playlist[newPathsIndex]);
            generate_display_objects(fileDisplays[newPathsIndex].second, largeFont, smallFont);
            ShaderManager::use(IMAGE);
            GLEC(glUniform1i(glGetUniformLocation(ShaderManager::program(IMAGE), "s_texture"), 1));
            fileDisplays[newPathsIndex].first = true;
        }

        currentPathsIndex = newPathsIndex;
        // end process input

        switch (newState) {
            case IDLE:
                if (playerState == PLAYING) {
                    pause(playerState, device);
                }
                blank_screen();
                break;
            case PLAYING:
                if (firstPlay) {
                    start_playlist_playback(deviceConfig, device, playlist);
                    firstPlay = false;
                }
                else {
                    play(playerState, device);
                }
                render_audio_file_display(fileDisplays[currentPathsIndex].second, largeFont, smallFont, true, camera);
                break;
            case PAUSED:
                if (playerState == PLAYING) {
                    pause(playerState, device);
                }
                render_audio_file_display(fileDisplays[currentPathsIndex].second, largeFont, smallFont, false, camera);
                break;
            case PLAYLIST_INFO:
                if (playerState == PLAYING) {
                    pause(playerState, device);
                }
                render_playlists_infO(playlistsDisplay, smallFont, camera);
                break;
            default:
                break;
        }

        playerState = newState;

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
    
    for (std::pair<bool, AudioFile>& fileDisplay : fileDisplays) {
        free_gl(fileDisplay.second);
        fileDisplay.first = false;
    }
    free_gl(largeFont);
    free_gl(smallFont);
    ShaderManager::delete_shaders();


    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}