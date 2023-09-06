#include "gladInclude.h"

#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<id3v2tag.h>
#include<mpegfile.h>
#include<id3v2frame.h>
#include<id3v2header.h>
#include <attachedpictureframe.h>
#include<cstdio>
#include<string.h>

#include "ShaderManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Text.h"
#include "Camera.h"

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

    GLFWwindow* window = glfwCreateWindow(800, 480, "window", nullptr, nullptr);
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
    
    string vShaderNegativeY_WorldSpaceStr = 
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "uniform mat4 m_mvp;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = m_mvp * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
        "   v_texCoord = a_texCoord;\n"
        "}\n";

    string textureF_ShaderStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main()\n"
        "{\n"
        "   gl_FragColor = texture2D(s_texture, v_texCoord);\n"
        "}\n";
    
    string textF_ShaderStr =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main()\n"
        "{\n"
        "   gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(s_texture, v_texCoord).a);\n"
        "}\n";

        

    ShaderManager::create_shader_from_string(vShaderTextureStr, textureF_ShaderStr, SHADER::TEXTURE);
    ShaderManager::create_shader_from_string(vShaderTextureStr, textF_ShaderStr, SHADER::TEXT);

    ShaderManager::use(TEXTURE);
    GLEC(glActiveTexture(GL_TEXTURE1));

    /* float vertices[] = {
        // positions        // texture coords
        760.0f, 440.0f, 1.0f, 1.0f, // top right
        760.0f, 160.0f, 1.0f, 0.0f, // bottom Bright
        480.0f, 160.0f, 0.0f, 0.0f, // bottom left
        480.0f, 440.0f, 0.0f, 1.0f  // top left 
    }; */
    float vertices[] = {
        // positions        // texture coords
        1.0f, 1.0f, 1.0f, 1.0f, // top right
        1.0f, 0.0, 1.0f, 0.0f, // bottom Bright
        0.0f, 0.0f, 0.0f, 0.0f, // bottom left
        0.0f, 1.0f, 0.0f, 1.0f  // top left 
    };
    uint indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    uint vbo, ebo;
    GLEC(glGenBuffers(1, &vbo));
    GLEC(glGenBuffers(1, &ebo));

    GLEC(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLEC(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GLEC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(TEXTURE), 0, "a_position"));

    GLEC(glEnableVertexAttribArray(1));
    GLEC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(2*sizeof(GL_FLOAT))));
    GLEC(glBindAttribLocation(ShaderManager::program(TEXTURE), 1, "a_texCoord"));
    GLEC(glLinkProgram(ShaderManager::program(TEXTURE)));


    ShaderManager::use(TEXTURE);
    // load and create a texture 
    // -------------------------
    uint texture1;
    // texture 1
    // ---------
    GLEC(glActiveTexture(GL_TEXTURE1));
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
    std::cout << "kc created texture id " << texture1 << "\n";
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    static const char *IdPicture = "APIC" ;
    TagLib::MPEG::File mpegFile("../tracks/I Know We'll Be Fine.mp3");
    TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
    TagLib::ID3v2::FrameList Frame ;
    TagLib::ID3v2::AttachedPictureFrame *PicFrame ;
    void *RetImage = NULL, *SrcImage ;
    unsigned long Size ;

    //FILE *jpegFile;
    //jpegFile = fopen("FromId3.jpg","wb");

    if ( id3v2tag ) {
        // picture frame
        Frame = id3v2tag->frameListMap()[IdPicture] ;
        if (!Frame.isEmpty()) {
            for(TagLib::ID3v2::FrameList::ConstIterator it = Frame.begin(); it != Frame.end(); ++it) {
                PicFrame = (TagLib::ID3v2::AttachedPictureFrame *)(*it) ;
                //  if ( PicFrame->type() ==
                //TagLib::ID3v2::AttachedPictureFrame::FrontCover)
                // extract image (in it’s compressed form)
                Size = PicFrame->picture().size() ;
                SrcImage = malloc ( Size ) ;
                if ( SrcImage ) {
                    unsigned char *data = stbi_load_from_memory((stbi_uc*)PicFrame->picture().data(), Size, &texWidth, &texHeight, &nrChannels, 0);
                    if (data) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                        glGenerateMipmap(GL_TEXTURE_2D);
                    }
                    else {
                        std::cout << "Failed to load texture" << std::endl;
                    }
                    //memcpy ( SrcImage, PicFrame->picture().data(), Size ) ;
                    //fwrite(SrcImage,Size,1, jpegFile);
                    //fclose(jpegFile);
                    free( SrcImage ) ;
                }
            }
        }
    }
    else {
        cout<< "id3v2 not present";
    }
    GLEC(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


    GLEC(glUniform1i(glGetUniformLocation(ShaderManager::program(TEXTURE), "s_texture"), 1));



    ShaderManager::use(TEXT);
    GLEC(glUniform1i(glGetUniformLocation(ShaderManager::program(TEXT), "s_texture"), 0));
    std::cout << "Creating fonts\n";
    FontData monolisaFontData = create_font("MonoLisa-Regular.ttf");
    FontData opensansFontData = create_font("OpenSans-Regular.ttf");

    std::cout << "\ngenerating text strip buffers\n";

    Text helloWorld;
    helloWorld.model.pos = vec2(40.0f, 360.0f);
    helloWorld.textStrip.points = layout_text("one", monolisaFontData); // quads are starting at 0,0
    generate_text_strip_buffers(helloWorld.textStrip);
    
    Text othertext;
    othertext.textStrip.points = layout_text("two", opensansFontData); // quads are starting at 0,0
    othertext.model.pos = vec2(40.0f, 160.0f);
    generate_text_strip_buffers(othertext.textStrip);

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.23f, 0.24f, 0.26f, 1.0f);

        
        render_text(helloWorld, monolisaFontData, camera);
        render_text(othertext, opensansFontData, camera);

        ShaderManager::use(TEXTURE);
        GLEC(glActiveTexture(GL_TEXTURE1));
        GLEC(glBindTexture(GL_TEXTURE_2D, texture1));
        GLEC(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

        GLEC(glEnableVertexAttribArray(0));
        GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)0));
        GLEC(glBindAttribLocation(ShaderManager::program(TEXTURE), 0, "a_position"));

        GLEC(glEnableVertexAttribArray(1));
        GLEC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(2*sizeof(GL_FLOAT))));
        GLEC(glBindAttribLocation(ShaderManager::program(TEXTURE), 1, "a_texCoord"));
        //GLEC(glLinkProgram(ShaderManager::program(TEXTURE)));
        
        glm::mat4 model = glm::mat4(1.0f);
        model = translate(model, vec3(480.0f, 160.0, 0.0f));
        model = glm::scale(model, glm::vec3(280.0, 280.0, 1.0));
        mat4 mvp = mat4(1.0f);
        mvp = camera.viewProjection * model;

        GLEC(glUniformMatrix4fv(glGetUniformLocation(ShaderManager::program(TEXTURE), "m_mvp"), 1, false, value_ptr(mvp)));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ShaderManager::delete_shaders();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}