#include "./Quad.h"

Quad::Quad(){}
Quad::~Quad(){}
void Quad::create(){
    float quad[] = {
        -1,  1,
         1,  1,
        -1, -1,
         1, -1
    };

    vbo.create(GL_ARRAY_BUFFER);
    vbo.setData(sizeof(quad), quad, GL_STATIC_DRAW);
}
void Quad::bind(){
    vbo.bind();

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));
}
void Quad::draw() {
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}