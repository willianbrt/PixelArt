#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../Shader/Shader.h"
#include <string>

class Texture {
private:
    bool initialized = false;
    ISurface* _surface;
    int _width, _height;
    Shader* _shader;
    GLuint texture;
    GLint uniformTexture;

    int id = 0;

    static int nTexture;
public:
    Texture();
    Texture(Shader* shader, std::string name);
    ~Texture();
    void active();
    void init(ISurface* surface);
    void upload(const Bounding& area);
    bool isInitialized();
};
#endif