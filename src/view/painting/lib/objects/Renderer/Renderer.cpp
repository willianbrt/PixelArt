#include "Renderer.h"

Renderer::Renderer(int width, int height) : _width(width), _height(height){
    createTexture();
    createShader();
    createQuad();
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
    // Point position = editor->getSketchPosition();
    // float scale = editor->getScale();


    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

    GLint pos = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos,2,GL_FLOAT,GL_FALSE,0,0);

    positionLocation = glGetUniformLocation(program, "pan");
    scaleLocation = glGetUniformLocation(program, "zoom");
    resolutionLocation = glGetUniformLocation(program, "resolution");
    texSizeLocation = glGetUniformLocation(program, "texSize");
    repeatLocation = glGetUniformLocation(program, "repeat");
    

    Point sketchPosition = { 10, 10 };
    int windowWidth = 250;
    int windowHeight = 500;
    float scale = 10.0f;

    glUniform2f(resolutionLocation, (float)windowWidth, (float)windowHeight);
    glUniform2f(texSizeLocation, (float)_width, (float)_height);
    glUniform2f(positionLocation, (float)sketchPosition.x, (float)sketchPosition.y);
    glUniform1f(scaleLocation, scale);
    glUniform2f(repeatLocation, 1.0f, 1.0f);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void Renderer::createShader()
{
    const char* vs =
    "attribute vec2 position;"
    "varying vec2 uv;"
    "uniform vec2 resolution;"
    "uniform vec2 texSize;"
    "uniform vec2 pan;"
    "uniform float zoom;"
    "uniform vec2 repeat;"
    "void main(){"
    "   vec2 pos = position*zoom + pan;"
    "   vec2 ndc = pos / resolution * 2.0;"
    "   gl_Position=vec4(vec2(ndc.x - 1.0, 1.0 - ndc.y),0.0,1.0);"
    "   uv = position / texSize*repeat;"
    "}";

    const char* fs =
    "precision mediump float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "void main(){"
    "gl_FragColor=texture2D(tex, uv);"
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
        0,0,
        (float)_width,0,
        0, (float)_height,
        (float)_width, (float)_height
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