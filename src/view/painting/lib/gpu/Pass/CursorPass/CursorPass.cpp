#include "./CursorPass.h"

CursorPass::CursorPass(EditorManager* manager, ToolManager* toolManager, ViewportContext* viewport){
    _manager = manager;
    _toolManager = toolManager;
    _viewport = viewport;
    
    const char* fs =R"(#version 300 es
        precision highp float;
        uniform sampler2D texB;
        in vec2 uv;
        in vec2 pixel;
        
        layout(std140) uniform GlobalData {
            vec2 resolution;
            vec2 cursor;
            vec2 cursorLocation;
            vec2 pan;
            vec2 zoom;
            vec2 repeat;
            float time;
        };
        
        uniform vec2 texSize;
        uniform vec2 brushSize;
        
        out vec4 fragColor;
        void main(){
            vec2 transformedUV = pixel / texSize;
            if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y < 0.0 || transformedUV.y > 1.0) discard;

            vec2 pixelHover = (pixel-cursorLocation) + floor(brushSize*0.5);
            vec2 hoverUV = pixelHover / brushSize;
            if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;

            fragColor = texture(texB, hoverUV).abgr;
        })";
    
    
    quad.create();
    shader.create(fs);
    shader.use();

    texSizeLocationH = glGetUniformLocation(shader.id(),"texSize");
    brushSizeLocation = glGetUniformLocation(shader.id(),"brushSize");
}
CursorPass::~CursorPass(){}

void CursorPass::init(){
    if (glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }
    glUniformBlockBinding(shader.id(),  glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);


    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        hover->pattern->width,
        hover->pattern->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        hover->pattern->buffer.data()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    initialized = true;
}
void CursorPass::draw(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    Surface* _surface = editor->getSurface();
    if(!_surface) return;
    
    hover = _toolManager->getCursorContext();
    if(hover == nullptr)  return;
    if(hover->pattern->buffer.size() == 0)  return;

    if(!initialized && hover->pattern->buffer.data() != nullptr){
         init();
    }
    if(!initialized) return;

    upload(Bounding());
    shader.use();

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.id(),"texB"), 2);

    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUniform2f(texSizeLocationH, _surface->getWidth(), _surface->getHeight());
    glUniform2f(brushSizeLocation, hover->pattern->width, hover->pattern->height);

    quad.draw();
}
void CursorPass::upload(Bounding area){
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        hover->pattern->width,
        hover->pattern->height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        hover->pattern->buffer.data()
    );
}