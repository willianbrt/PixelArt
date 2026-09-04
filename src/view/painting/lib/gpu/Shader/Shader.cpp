#include "./Shader.h"

const char* Shader::vertex_data = R"(#version 300 es
    precision highp float;
    in vec2 position;

    out vec2 uv;
    out vec2 pixel;

    layout(std140) uniform GlobalData {
        vec2 resolution;
        vec2 cursor;
        vec2 cursorLocation;
        vec2 pan;
        vec2 zoom;
        vec2 repeat;
        float time;
    };

    void main(){
        uv = position * 0.5 + 0.5;
        uv = vec2(uv.x, 1.0 - uv.y) * resolution;
        
        pixel = (uv - pan) / (zoom /repeat);

        gl_Position = vec4(position,0.0,1.0);
    }
)";
GLuint Shader::v = 0;
Shader::Shader(){
    if(!glIsShader(v)){
        v = compile(GL_VERTEX_SHADER, vertex_data);
        printf("\n");
    }
}
Shader::~Shader(){}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok;
    glGetShaderiv(s,GL_COMPILE_STATUS,&ok);

    if(!ok){
        char log[512];
        glGetShaderInfoLog(s,512,nullptr,log);
        printf("Shader error: %s\n",log);
    }
    return s;
}

void Shader::create(const char* vs, const char* fs) {
    GLuint custom_vertex = compile(GL_VERTEX_SHADER, vs);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs);

    _program = glCreateProgram();
    glAttachShader(_program, custom_vertex);
    glAttachShader(_program, f);
    glLinkProgram(_program);


    GLint success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);

    if (!success) {
        char log[512];
        glGetProgramInfoLog(_program, 512, NULL, log);
        printf("PROGRAM LINK ERROR:\n%s\n", log);
    }

    // glDeleteShader(custom_vertex);
    // glDeleteShader(f);
}
void Shader::create(const char* fs) {
    // v = compile(GL_VERTEX_SHADER, vs);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs);

    _program = glCreateProgram();
    glAttachShader(_program, v);
    glAttachShader(_program, f);
    glLinkProgram(_program);

    GLint success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);

    if (!success) {
        char log[512];
        glGetProgramInfoLog(_program, 512, NULL, log);
        printf("PROGRAM LINK ERROR:\n%s\n", log);
    }
    // glDeleteShader(v);
    // glDeleteShader(f);
}
// void Shader::createVertex(const char* vs, const char* fs) {
//     GLuint v = compile(GL_VERTEX_SHADER, vs);
//     glDeleteShader(v);
// }

void Shader::use() {
    glUseProgram(_program);
}
GLuint Shader::id(){ return _program; }
void Shader::setInt(const char* name, int v) {
    glUniform1i(glGetUniformLocation(_program, name), v);
}