#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

void calculate_projection(Camera& camera);
void calculate_view(Camera& camera);

void calculate_view_projection(Camera& camera) {
    calculate_projection(camera);
    calculate_view(camera);
    camera.viewProjection = camera.projection * camera.view;
}

void calculate_projection(Camera& camera) {
    float halfScreemWidth  = camera.screenWidth;
    float halfScreemHeight = camera.screenHeight;

    camera.projection = glm::ortho(
        0.0f,  
        camera.screenWidth, 
        0.0f,
        camera.screenHeight,   
        0.1f,
        100.0f
    );

}

void calculate_view(Camera& camera) {
    camera.view = glm::lookAt(
        camera.pos,
        camera.lookAt,
        camera.up
    );

    //camera.view = mat4(1.0f);
}