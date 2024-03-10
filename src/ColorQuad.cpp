#include "ColorQuad.h"
#include "ShaderManager.h"

#include <glm/gtx/rotate_vector.hpp>

void init(ColorQuad& quad, vec2 pos, vec2 size, vec4 color)
{
    quad.pos = pos;
    quad.size = size;


    ShaderManager::use(COLOR);
    float vertices[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    uint indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    GLEC(glGenBuffers(1, &quad.vertexBufferID));
    GLEC(glGenBuffers(1, &quad.elementBufferID));

    GLEC(glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBufferID));
    GLEC(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.elementBufferID));
    GLEC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(COLOR), 0, "a_position"));

    GLEC(glLinkProgram(ShaderManager::program(COLOR)));
}

void render_color_quad(ColorQuad& quad, const Camera& camera) {
    //std::cout << "rendering quad\n";
    ShaderManager::use(COLOR);
    GLEC(glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBufferID));
    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.elementBufferID));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(COLOR), 0, "a_position"));

    //std::cout << "rendering quad at pos " << vec_string(quad.pos) << ", size" << vec_string(quad.size) << ", vbid " << quad.vertexBufferID << " and ebid " << quad.elementBufferID << "\n";

    glm::mat4 model = glm::mat4(1.0f);
    model = translate(model, vec3(quad.pos.x, quad.pos.y, 0.0f));
    model = glm::scale(model, glm::vec3(quad.size.x, quad.size.y, 1.0));

    mat4 mvp = mat4(1.0f);
    mvp = camera.viewProjection * model;

    GLEC(glUniformMatrix4fv(glGetUniformLocation(ShaderManager::program(COLOR), "m_mvp"), 1, false, value_ptr(mvp)));
    //GLEC(glUniform4fv(glGetUniformLocation(ShaderManager::program(COLOR), "v_color"), 1, value_ptr(quad.color)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}