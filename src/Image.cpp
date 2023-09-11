#include "Image.h"

#include "definitions.h"
#include "ShaderManager.h"

#include <glm/gtx/rotate_vector.hpp>

#include<id3v2tag.h>
#include<mpegfile.h>
#include<id3v2frame.h>
#include<id3v2header.h>
#include <attachedpictureframe.h>
#include "stb_image.h"

void generate_image_buffers(Image& image) {
    ShaderManager::use(IMAGE);
    GLEC(glActiveTexture(GL_TEXTURE1));
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

    GLEC(glGenBuffers(1, &image.vertexBufferID));
    GLEC(glGenBuffers(1, &image.elementBufferID));

    GLEC(glBindBuffer(GL_ARRAY_BUFFER, image.vertexBufferID));
    GLEC(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, image.elementBufferID));
    GLEC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(IMAGE), 0, "a_position"));

    GLEC(glEnableVertexAttribArray(1));
    GLEC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(2*sizeof(GL_FLOAT))));
    GLEC(glBindAttribLocation(ShaderManager::program(IMAGE), 1, "a_texCoord"));
    GLEC(glLinkProgram(ShaderManager::program(IMAGE)));
}

void set_image_texture_from_audio_file(Image& image, std::string audioFileName) {
    ShaderManager::use(IMAGE);
    // load and create a texture 
    // -------------------------
    GLEC(glActiveTexture(GL_TEXTURE1));
    glGenTextures(1, &image.textureID);
    glBindTexture(GL_TEXTURE_2D, image.textureID); 
    std::cout << "created image texture id " << image.textureID << "\n";
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
    TagLib::MPEG::File mpegFile(audioFileName.c_str());
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
                    free(SrcImage);
                }
            }
        }
    }
    else {
        std::cout << "id3v2 not present";
    }
    GLEC(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void render_image(const Image& image, const Camera& camera) {
    ShaderManager::use(IMAGE);
    GLEC(glActiveTexture(GL_TEXTURE1));
    GLEC(glBindTexture(GL_TEXTURE_2D, image.textureID));
    GLEC(glBindBuffer(GL_ARRAY_BUFFER, image.vertexBufferID));
    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, image.elementBufferID));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(IMAGE), 0, "a_position"));

    GLEC(glEnableVertexAttribArray(1));
    GLEC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(2*sizeof(GL_FLOAT))));
    GLEC(glBindAttribLocation(ShaderManager::program(IMAGE), 1, "a_texCoord"));
    
    glm::mat4 model = glm::mat4(1.0f);
    model = translate(model, vec3(image.model.pos.x, image.model.pos.y, 0.0f));
    model = glm::scale(model, glm::vec3(image.size.x, image.size.y, 1.0));

    mat4 mvp = mat4(1.0f);
    mvp = camera.viewProjection * model;

    GLEC(glUniformMatrix4fv(glGetUniformLocation(ShaderManager::program(IMAGE), "m_mvp"), 1, false, value_ptr(mvp)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}