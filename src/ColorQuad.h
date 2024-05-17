#pragma once

#include "definitions.h"
#include "Camera.h"

struct ColorQuad {
    vec2 pos;
    vec2 size;
    vec4 color;
    uint vertexBufferID = 0;
    uint elementBufferID = 0;
};

void init(ColorQuad& quad, vec2 pos, vec2 size, vec4 color);
void render_color_quad(const ColorQuad& quad, const Camera& camera);