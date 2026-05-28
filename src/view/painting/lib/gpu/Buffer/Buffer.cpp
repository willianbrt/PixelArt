#include "./Buffer.h"

Buffer::Buffer(){}
Buffer::~Buffer(){}
void Buffer::create(GLenum type){
    _type = type;
    glGenBuffers(1,&_id);
}
void Buffer::setData(size_t size, const void* data, GLenum usage){
    bind();
    glBufferData(_type, size, data, usage);
}
void Buffer::update(size_t offset, size_t size, const void* data, GLenum usage){
    bind();
    glBufferSubData(_type, offset, size, data);
}
void Buffer::bind(){
    glBindBuffer(_type,_id);
}
GLuint Buffer::getId() const { return _id; }