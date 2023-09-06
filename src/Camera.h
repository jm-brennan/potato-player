#pragma once

#include <glm/glm.hpp>
#include "definitions.h"

struct Camera {
    float screenWidth = 0;
    float screenHeight = 0;

    mat4 viewProjection;
    mat4 projection;
    mat4 view;
   
    vec3 pos;
    vec3 lookAt;
    vec3 up;
};

void calculate_view_projection(Camera& camera);