#pragma once

#include "Camera.h"
#include "Model.h"

struct Image {
    uint textureID;
    uint vertexBufferID;
    uint elementBufferID;
    Model model;
    float size = 0.0;
};

void generate_image_buffers(Image& image);
void set_image_texture_from_audio_file(Image& image, std::string audioFileName);
void render_image(Image& image, Camera& camera);