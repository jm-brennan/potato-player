#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"

int make_texture(const char* filename, bool flip) {
    uint texture;
    GLEC(glGenTextures(1, &texture));
    GLEC(glBindTexture(GL_TEXTURE_2D, texture));
    
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    int width;
    int height;
    int numChannels;

    stbi_set_flip_vertically_on_load(flip);

    unsigned char* data = stbi_load(filename, &width, &height, &numChannels, 0);

    if (data) {
        GLEC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        GLEC(glGenerateMipmap(GL_TEXTURE_2D));
        printf("Generated texture for file: %s with numChannels: %d\n", filename, numChannels);
    }
    else {
        printf("Error reading data frome file: %s\n", filename);
    }

    stbi_image_free(data);
    return texture;
}