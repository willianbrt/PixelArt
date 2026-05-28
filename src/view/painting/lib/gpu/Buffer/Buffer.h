#ifndef BUFFER_H
#define BUFFER_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

class Buffer {
private:
    GLuint _id = 0;
    GLenum _type;
public:
    Buffer();
    ~Buffer();
    void create(GLenum type);
    void bind();
    void setData(size_t size, const void* data, GLenum usage);
    void update(size_t offset, size_t size, const void* data, GLenum usage);
    GLuint getId() const;
};
#endif