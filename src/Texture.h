#pragma once

#include "gladInclude.h"

#include "definitions.h"

//int make_texture(const char* filename, bool flip = true);

#pragma pack(push, 1)
struct TexturePoint {
    vec2 vertex;
    vec2 texture;
};
#pragma pack(pop)