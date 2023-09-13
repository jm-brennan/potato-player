#pragma once

#include "Camera.h"
#include "Model.h"

struct Image {
    uint textureID;
    uint vertexBufferID;
    uint elementBufferID;
    Model model;
    vec2 size;
};

void generate_image_buffers(Image& image);
void free_gl(Image& image);
void set_image_texture_from_audio_file(Image& image, std::string audioFileName);
void render_image(const Image& image, const Camera& camera);