#include "./CursorPass.h"

CursorPass::CursorPass(EditorManager* manager, ToolManager* toolManager, ToolSettings* toolSettings, ViewportContext* viewport){
    _manager = manager;
    _toolManager = toolManager;
    _toolSettings = toolSettings;
    _viewport = viewport;
    
    const char* fs =R"(#version 300 es
        precision highp float;
        uniform sampler2D texPattern;
        uniform sampler2D texShape;
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
        
        uniform vec2 editorSize;
        uniform vec2 texPatternSize;
        uniform vec2 texShapeSize;
        uniform vec2 brushSize;
        uniform vec4 color;
        uniform float hardness;
        uniform int alignMode;
        uniform int fillMode;
        
        out vec4 fragColor;
        void main(){
            vec2 transformedUV = pixel / editorSize;
            if(transformedUV.x < 0.0 || transformedUV.x > repeat.x || transformedUV.y < 0.0 || transformedUV.y > repeat.y) discard;

            vec2 pixelShape = (pixel-cursorLocation) + floor(texShapeSize*0.5);
            vec2 hoverUV = pixelShape / texShapeSize;
            if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;
            

            float shapeMask = texture(texShape, hoverUV).r*hardness;

            vec3 c;
            if(fillMode == 0){ // SOLID COLOR
                c = vec3(color.r,color.g,color.b);
                fragColor = vec4(c, shapeMask);
            } else if(fillMode == 1) { // MASK
                vec2 pc;
                if(alignMode == 0){ // BRUSHING
                    pc = fract(((pixel-cursorLocation) + floor(texPatternSize*0.5)) / texPatternSize);
                } else if(alignMode == 1){ // LOCAL
                    pc = fract(((pixel-cursorLocation) + floor(texPatternSize*0.5)) / texPatternSize);
                } else if(alignMode == 2){ // SOURCE
                    pc = fract(pixel / texPatternSize);
                }

                float patternMask = texture(texPattern, pc).r; // stamp

                c = vec3(color.r,color.g,color.b);
                fragColor = vec4(c, shapeMask*patternMask);
            } else if(fillMode == 2) { // stamp
                c = texture(texShape, hoverUV).gba;
                fragColor = vec4(c, shapeMask);
            }

        })";
    
    
    quad.create();
    shader.create(fs);
    shader.use();

    glUniformBlockBinding(shader.id(),  glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);
    editorSizeLocation = glGetUniformLocation(shader.id(),"editorSize");
    
    texturePattern = Texture(2, &shader, "texPattern");
    texPatternSizeLocationH = glGetUniformLocation(shader.id(),"texPatternSize");
    
    textureShape = Texture(3, &shader, "texShape");
    texShapeSizeLocationH = glGetUniformLocation(shader.id(),"texShapeSize");


    brushSizeLocation = glGetUniformLocation(shader.id(),"brushSize");
    brushColorLocation = glGetUniformLocation(shader.id(),"color");
    hardnessLocation = glGetUniformLocation(shader.id(),"hardness");
    fillModeLocation = glGetUniformLocation(shader.id(),"fillMode");
    alignModeLocation = glGetUniformLocation(shader.id(),"alignMode");
}
CursorPass::~CursorPass(){}

void CursorPass::render(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    Surface* _surface = editor->getSurface();
    if(!_surface) return;
    
    CursorContext* cursor = _toolManager->getCursorContext();

    ISurface* pattern = _toolSettings->brushContext.getPattern();
    ISurface* shape = _toolSettings->brushContext.getShape();

    if(_pattern != pattern){
        _pattern = pattern;
        texturePattern.init(_pattern);
        if(!textureShape.isInitialized()) return;
    }

    if(_shape != shape){
        _shape = shape;
        textureShape.init(_shape);
        if(!texturePattern.isInitialized()) return;
    }

    shader.use();
    textureShape.active();
    texturePattern.active();
    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUniform2f(editorSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform2f(texShapeSizeLocationH, _shape == nullptr ? 0 : _shape->getWidth(), _shape == nullptr ? 0 :_shape->getHeight());
    glUniform2f(texPatternSizeLocationH, _pattern == nullptr ? 0 : _pattern->getWidth(), _pattern == nullptr ? 0 :_pattern->getHeight());

    glUniform2f(brushSizeLocation, (float)_toolSettings->brushContext.getSize(), (float)_toolSettings->brushContext.getSize());
    
    glUniform1i(fillModeLocation,  (int)_toolSettings->brushContext.getFillMode());
    glUniform1i(alignModeLocation, (int)_toolSettings->brushContext.getAlignMode());

    unsigned int color = _toolSettings->brushContext.getColor();
    float a = (color & 0xFF) / 255.0f;      
    float b = (color >> 8 & 0xFF) / 255.0f; 
    float g = (color >> 16 & 0xFF) / 255.0f;
    float r = (color >> 24 & 0xFF) / 255.0f;

    glUniform4f(brushColorLocation, r,g,b,a);
    glUniform1f(hardnessLocation, _toolSettings->brushContext.getHardness());

    quad.draw();
}