#pragma once

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

using namespace glm;


struct Camera {
public:
    Camera(float screenWidth, float screenHeight, vec3 pos, vec3 lookAt, vec3 up);

    void update_bounds(float newScreenWidth, float newScreenHeight);

    void calculate_projection();
    void calculate_view(vec3 pos, vec3 lookat, vec3 up);

    //void slide_zoom(float multiplier);

    mat4 get_projection();
    mat4 get_view();

private:
    float screenWidth, screenHeight, squareBound;
    // use a different projection matrix to render worldspace or gui elements in screenspace
    mat4 projection;
    mat4 view;
    vec3 pos, lookAt, up;
};