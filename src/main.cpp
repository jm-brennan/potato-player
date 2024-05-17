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

#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "miniaudio.h"

#include "Playlist.h"
#include "AudioFileDisplay.h"
#include "audio_player.h"
#include "gpio.h"
#include "ColorQuad.h"

using namespace std;

const uint NUM_PLAYLISTS = 6;

extern std::atomic<uint> currentTrackFrame;
extern std::atomic<uint> currentTrackLength;

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

void render_playlists_infO(const std::map<uint, Text>& playlists, FontList& fonts, Camera& camera) {
    for (const std::pair<uint, Text>& playlist : playlists) {
        render_text(playlist.second, fonts[FontIndex::MEDIUM][32], camera);
    }
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
        //std::cout << "Already playing\n";
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

GLFWwindow* window_init() {
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

    return window;
}

void opengl_init() {
    #ifdef USE_OPENGL_ES
    gladLoadGLES2(glfwGetProcAddress);
    #else
    gladLoadGL(glfwGetProcAddress);
    #endif
    
    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(void) {
    GLFWwindow* window = window_init();
    opengl_init();

    Camera camera;
    camera.screenWidth = 800;
    camera.screenHeight = 480;
    camera.pos = vec3(0.0, 0.0, 3.0);
    camera.lookAt = vec3(0.0, 0.0, 0.0);
    camera.up = vec3(0.0, 1.0, 0.0);
    calculate_view_projection(camera);

    //shaders_init();

    string vShaderTextureStr = 
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "uniform mat4 m_mvp;\n"
        "void main() {\n"
        "   gl_Position = m_mvp * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
        "   v_texCoord = a_texCoord;\n"
        "}\n";
    
    string fShaderTextureStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main() {\n"
        "   gl_FragColor = texture2D(s_texture, v_texCoord);\n"
        "}\n";
    
    string fShaderTextStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(s_texture, v_texCoord).a);\n"
        "}\n";
    
    string fShaderColorStr =
        "precision mediump float;\n"
        "//uniform vec4 v_color;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n";


    shader::create_shader_from_string(vShaderTextureStr, fShaderTextStr, SHADER::TEXT);

    //shader::use(TEXT);
    //GLEC(glUniform1i(glGetUniformLocation(shader::program(TEXT), "s_texture"), 0));
    //std::cout << "Creating fonts at indices: " << FontIndex::LARGE << ", " << FontIndex::MEDIUM << ", " << FontIndex::SMALL_ITALIC << "\n";

    freetype_init();

    FontList fonts;
    font_init(fonts[FontIndex::LARGE][64], "Poppins-Medium.ttf", 64, {});
    font_init(fonts[FontIndex::MEDIUM][32], "Poppins-Light.ttf", 32, {});
    font_init(fonts[FontIndex::SMALL_ITALIC][24], "Poppins-LightItalic.ttf", 24, {});
    font_init(fonts[FontIndex::MONO][20], "ChivoMono-Light.ttf", 20, {});
    font_init(fonts[FontIndex::JAPANESE][20], "NotoSansJP-Regular.ttf", 20, {});

    shader::create_shader_from_string(vShaderTextureStr, fShaderColorStr, SHADER::COLOR);

    shader::create_shader_from_string(vShaderTextureStr, fShaderTextureStr, SHADER::IMAGE);

    State playerState = PLAYING;//PLAYLIST_INFO;

    Playlists playlists = parse_playlists();

    shader::use(TEXT);
    std::cout << "Creating playlists display\n";
    std::map<uint, Text> playlistsDisplay;
    for (const std::pair<uint, Playlist>& playlist : playlists) {
        uint playlistID = playlist.first;
        if (playlistID > 0 && playlistID <= NUM_PLAYLISTS) {
            text_init(playlistsDisplay[playlistID], wsconverter.from_bytes(playlist.second.name), 32);
            layout_text(playlistsDisplay[playlistID], fonts, FontIndex::MEDIUM, playlistsDisplay[playlistID].fontSizePx);
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

    AudioFileDisplay currentTrackDisplays;
    uint currentPathsIndex = 0;
    audio_file_init(currentTrackDisplays, playlist[currentPathsIndex]);
    generate_display_objects(currentTrackDisplays, fonts);
    shader::use(IMAGE);
    GLEC(glUniform1i(glGetUniformLocation(shader::program(IMAGE), "s_texture"), 1));

    ma_device_config deviceConfig;
    ma_device device;

    init_decoder_config();

    bool firstPlay = true;

    uint framesToSwitch = 140;
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
        State newState = playerState;//(State)((uint)(frameCounter / framesToSwitch) % 4);
        //std::cout << "current state is " << StateString(playerState)
         //         << ", state to switch to is " << StateString(newState) << "\n";

        int newPathsIndex = pathsIndex.load();
        if (currentPathsIndex != newPathsIndex) {
            free_gl(currentTrackDisplays);
            std::cout << "generating display objects for " << paths[newPathsIndex] << "\n";
            audio_file_init(currentTrackDisplays, playlist[newPathsIndex]);
            generate_display_objects(currentTrackDisplays, fonts);
            shader::use(IMAGE);
            GLEC(glUniform1i(glGetUniformLocation(shader::program(IMAGE), "s_texture"), 1));
        }

        currentPathsIndex = newPathsIndex;
        // end process input

        uint currentLength = currentTrackLength.load();
        //std::cout << "current length is " << currentLength << "\n";
        update_playback_progress(currentTrackDisplays, currentTrackFrame.load(), currentLength, fonts);

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
                render_audio_file_display(currentTrackDisplays, fonts, true, camera);
                break;
            case PAUSED:
                if (playerState == PLAYING) {
                    pause(playerState, device);
                }
                render_audio_file_display(currentTrackDisplays, fonts, false, camera);
                break;
            case PLAYLIST_INFO:
                if (playerState == PLAYING) {
                    pause(playerState, device);
                }
                render_playlists_infO(playlistsDisplay, fonts, camera);
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
    
    free_gl(currentTrackDisplays);
    for (uint i = 0; i < FontIndex::NUM_FONTS; ++i) {
        for (std::pair<const uint, FontData>& font : fonts[i]) {
            FT_Done_Face(font.second.face);
            free_gl(font.second);
        }
    }
    shader::delete_shaders();

    shutdown_freetype();
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}