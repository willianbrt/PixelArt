#include "./Texture.h"

int Texture::nTexture = 0;

Texture::Texture(){}
Texture::Texture(Shader* shader, std::string name){
    id = nTexture;    
    uniformTexture = glGetUniformLocation(shader->id(), name.c_str());
    _shader = shader;

    nTexture++;
}
Texture::~Texture(){
    nTexture--;
}

void Texture::active(){
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(uniformTexture, 0);
}

void Texture::init(ISurface* surface){
    printf("i texture: %i\n", id);
    _surface = surface;
    _width = _surface->getWidth();
    _height = _surface->getHeight();
    
    if (glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _surface->getWidth(),
        _surface->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _surface->getBuffer()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    initialized = true;
}

void Texture::upload(const Bounding& area){
    if(_width != _surface->getWidth() || _height != _surface->getHeight()) {
        init(_surface);
        return;
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, _surface->getWidth());
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, area.start.x);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, area.start.y);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        area.start.x,
        area.start.y,
        area.getWidth(),
        area.getHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _surface->getBuffer()
    );

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}
bool Texture::isInitialized(){
    return initialized;
}