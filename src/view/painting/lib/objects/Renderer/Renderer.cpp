#include "Renderer.h"

Renderer::Renderer(int width, int height) : _width(width), _height(height){
    createContext();

    createTexture();
    createShader();
    createQuad();
    printf("\n");
}

void Renderer::createContext()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.alpha = true;
    attrs.depth = true;
    attrs.stencil = false;
    attrs.antialias = false;
    attrs.majorVersion = 2;

    context = emscripten_webgl_create_context("#painting", &attrs);

    emscripten_webgl_make_context_current(context);

    emscripten_set_canvas_element_size("#painting", _width, _height);
    glViewport(0,0, _width, _height);
}

void Renderer::createTexture()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _width,
        _height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Renderer::uploadSurface(Bounding area, Surface* surface){
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->getWidth());

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        area.start.x,
        area.start.y,
        area.getWidth(),
        area.getHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->getBuffer() + (area.start.x + area.start.y*surface->getWidth())
    );
    
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}
void Renderer::draw()
{
    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER,vbo);

    GLint pos = glGetAttribLocation(program,"position");

    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos,2,GL_FLOAT,GL_FALSE,0,0);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void Renderer::createShader()
{
    const char* vs =
    "attribute vec2 position;"
    "varying vec2 uv;"
    "void main(){"
    "uv=(position+1.0)*0.5;"
    "gl_Position=vec4(position,0.0,1.0);"
    "}";

    const char* fs =
    "precision mediump float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "void main(){"
    "gl_FragColor=texture2D(tex, vec2(uv.x, 1.0 - uv.y));"
    "}";

    GLuint v = compile(GL_VERTEX_SHADER,vs);
    GLuint f = compile(GL_FRAGMENT_SHADER,fs);

    program = glCreateProgram();

    glAttachShader(program,v);
    glAttachShader(program,f);

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program,GL_LINK_STATUS,&success);

    if(!success)
    {
        char log[512];
        glGetProgramInfoLog(program,512,nullptr,log);
        printf("Program link error: %s\n",log);
    }
}

void Renderer::createQuad()
{
    float quad[] = {
        -1,-1,
         1,-1,
        -1, 1,
         1, 1
    };

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quad),
        quad,
        GL_STATIC_DRAW
    );
}
GLuint Renderer::compile(GLenum type,const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);


    GLint success;
    glGetShaderiv(s,GL_COMPILE_STATUS,&success);

    if(!success)
    {
        char log[512];
        glGetShaderInfoLog(s,512,nullptr,log);
        printf("Shader compile error: %s\n",log);
    }

    return s;
}


void Renderer::render(Bounding area, Surface* surface){
    uploadSurface(area, surface);
    draw();
}