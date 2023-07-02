#include "Camera.h"

Camera::Camera(float screenWidth, float screenHeight, vec3 pos, vec3 lookat, vec3 up) 
    : screenWidth(screenWidth), screenHeight(screenHeight), pos(pos), lookAt(lookAt), up(up) {
    
    calculate_projection();
    calculate_view(pos, lookat, up);
    
    //screenToWorld = inverse(view) * inverse(projection);
    printf("created view mat: %s\n", glm::to_string(view).c_str());
    printf("created projection mat: %s\n", glm::to_string(projection).c_str());
}

void Camera::update_bounds(float newScreenWidth, float newScreenHeight) {
    screenWidth = newScreenWidth;
    screenHeight = newScreenHeight;
    calculate_projection();
    //screenToWorld = inverse(view) * inverse(projection);
}

void Camera::calculate_projection() {
    projection = ortho(
        0.0f,       // left  
        screenWidth,       // right 
        screenHeight,       // top   
        0.0f,       // bottom
        0.0f,       // near clip
        100.0f      // far clip
    );
}

void Camera::calculate_view(vec3 pos, vec3 lookat, vec3 up) {
    view = glm::lookAt(
        pos,
        lookat,
        up
    );
}

mat4 Camera::get_projection() { return projection; }
mat4 Camera::get_view() { return view; }