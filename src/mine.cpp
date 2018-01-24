#include "mine.h"
#include "main.h"

Mine::Mine(float x, float y, color_t color) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->speed = 0;
    static const GLfloat vertex_buffer_data[] = {
        -0.8, -0.2, 0, // vertex 1
        0.8,  -0.2, 0, // vertex 2
        0.8,  0.2, 0, // vertex 3

        0.8,  0.2, 0, // vertex 3
        -0.8, 0.2, 0, // vertex 4
        -0.8, -0.2, 0, // vertex 1

        -0.8,0.2,0,
        -0.6,0.3,0,
        -0.4,0.2,0,

        -0.4,0.2,0,
        -0.2,0.3,0,
        -0.0,0.2,0,

        -0.0,0.2,0,
        0.2,0.3,0,
        0.4,0.2,0,

        0.4,0.2,0,
        0.6,0.3,0,
        0.8,0.2,0,


    };

    this->object = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color, GL_FILL);
}

void Mine::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate          = rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Mine::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

bounding_box_t Mine::bounding_box() {
    float x = this->position.x, y = this->position.y;
    bounding_box_t bbox = { x, y, 1.6, 0.4 };
    return bbox;
}
void Mine::tick() {
    this->position.x -= this->speed;
}
