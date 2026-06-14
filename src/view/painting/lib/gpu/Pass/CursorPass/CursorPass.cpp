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

    glUniformBlockBinding(shader.id(),  glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);
    texture = Texture(2, &shader, "texB");
    texSizeLocationH = glGetUniformLocation(shader.id(),"texSize");
    brushSizeLocation = glGetUniformLocation(shader.id(),"brushSize");
}
CursorPass::~CursorPass(){}

void CursorPass::render(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    Surface* _surface = editor->getSurface();
    if(!_surface) return;
    
    CursorContext* cursor = _toolManager->getCursorContext();
    if(cursor == nullptr)  return;
    if(cursor->pattern == nullptr)  return;
    if(!cursor->pattern->getBuffer())  return;

    texture.init(cursor->pattern);
    if(!texture.isInitialized()) return;

    shader.use();
    texture.active();
    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUniform2f(texSizeLocationH, _surface->getWidth(), _surface->getHeight());
    glUniform2f(brushSizeLocation, cursor->pattern->getWidth(), cursor->pattern->getHeight());

    quad.draw();
}