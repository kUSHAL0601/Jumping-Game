#include "ball.h"
#include "main.h"


Ball::Ball(float x, float y, color_t color) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->speedx = 0;
    this->speedy=0;
    this->accelaration=0;
    this->vertices=102;
    GLfloat vertex_buffer_data[100001] = {};
    for(int i=0;i<vertices;i+=1)
    {
        vertex_buffer_data[9*i]=0.2*cos((float)(2*M_PI*i)/vertices);
        vertex_buffer_data[9*i+1]=0.2*sin((float)(2*M_PI*i)/vertices);
        vertex_buffer_data[9*i+2]=0.0f;

        vertex_buffer_data[9*i+3]=0.2*cos((float)(2*M_PI*(i+1)/vertices));
        vertex_buffer_data[9*i+4]=0.2*sin((float)(2*M_PI*(i+1)/vertices));
        vertex_buffer_data[9*i+5]=0.0f;

        vertex_buffer_data[9*i+6]=0.0f;
        vertex_buffer_data[9*i+7]=0.0f;
        vertex_buffer_data[9*i+8]=0.0f;
        //std::cout<<vertex_buffer_data[9*i]<<" "<<vertex_buffer_data[9*i+1]<<" "<<vertex_buffer_data[9*i+2]<<std::endl;
        for(int j=0;j<9;j++)
        {
            if((fabs(vertex_buffer_data[9*i+j]))<=0.0000001)
                vertex_buffer_data[9*i+j]=0.0  ;
        }
        //std::cout<<vertex_buffer_data[9*i]<<" "<<vertex_buffer_data[9*i+1]<<" "<<vertex_buffer_data[9*i+2]<<std::endl;
    }

    this->object1 = create3DObject(GL_TRIANGLES, (9*(int)this->vertices)/6, vertex_buffer_data, color,GL_FILL);
    this->object2 = create3DObject(GL_TRIANGLES, (9*(int)this->vertices)/6, vertex_buffer_data + (9*(int)this->vertices)/6, COLOR_BLACK,GL_FILL);
    this->object3 = create3DObject(GL_TRIANGLES, (9*(int)this->vertices)/6, vertex_buffer_data + 2*(9*(int)this->vertices)/6, color,GL_FILL);
    this->object4 = create3DObject(GL_TRIANGLES, (9*(int)this->vertices)/6, vertex_buffer_data + 3*(9*(int)this->vertices)/6, COLOR_BLACK,GL_FILL);
    this->object5 = create3DObject(GL_TRIANGLES, (9*(int)this->vertices)/6, vertex_buffer_data + 4*(9*(int)this->vertices)/6, color,GL_FILL);
    this->object6 = create3DObject(GL_TRIANGLES, (9*(int)this->vertices)/6, vertex_buffer_data + 5*(9*(int)this->vertices)/6, COLOR_BLACK,GL_FILL);
}

void Ball::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate          = rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    draw3DObject(this->object1);
    draw3DObject(this->object2);
    draw3DObject(this->object3);
    draw3DObject(this->object4);
    draw3DObject(this->object5);
    draw3DObject(this->object6);
}

void Ball::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Ball::tick() {
    this->position.x -= this->speedx;
    this->position.y -= this->speedy;
    this->speedy -=this->accelaration;
}
void Ball::tick_jump(float dist) {
    if(this->position.x-this->speedx<=-2.0)this->speedx=0;
    else this->position.x -= this->speedx;
    if(this->position.y-this->speedy<=0.0)this->speedy=0;
    else this->position.y -= this->speedy;
}

bounding_box_t Ball::bounding_box() {
    float x = this->position.x, y = this->position.y;
    bounding_box_t bbox = { x, y, 0.4, 0.4 };
    return bbox;
}
