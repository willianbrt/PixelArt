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
        uniform bool isSolidColor;
        uniform bool isPatternEnable;
        uniform int typeBrushTip;
        
        out vec4 fragColor;


        vec4 getPatternPixel(sampler2D tex, vec2 texSize){
            if(alignMode == 0 || alignMode == 1){ // BRUSHING  // LOCAL
                return texture(tex, fract(((pixel-cursorLocation) + floor(texSize*0.5)) / texSize));
            } else if(alignMode == 2){ // SOURCE
                return texture(tex, fract(pixel / texSize));
            }
            return texture(tex, fract(pixel / texSize));
        }

        void main(){
            vec2 transformedUV = pixel / editorSize;
            if(transformedUV.x < 0.0 || transformedUV.x > repeat.x || transformedUV.y < 0.0 || transformedUV.y > repeat.y) discard;

            vec2 pixelShape = (pixel-cursorLocation) + floor(texShapeSize*0.5);
            vec2 hoverUV = pixelShape / texShapeSize;
            if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;
            

            float shapeMask = texture(texShape, hoverUV).r*hardness;

            vec3 c = color.rgb;
            float patternMask = 1.0;
            if(isPatternEnable) {
                vec4 patternPixel;
                if(texPatternSize.x > 0.0 && texPatternSize.y > 0.0 ){
                    patternPixel = getPatternPixel(texPattern, texPatternSize);
                } else {
                    patternPixel = getPatternPixel(texShape, texShapeSize);
                }
                patternMask = patternPixel.r;

                if(!isSolidColor) {
                    c = patternPixel.agb;
                }
            } else{
                if(!isSolidColor && typeBrushTip != 0){
                    c = texture(texShape, hoverUV).agb;
                }
            }
            fragColor = vec4(c, shapeMask*patternMask);

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
    isSolidColorLocation = glGetUniformLocation(shader.id(),"isSolidColor");
    alignModeLocation = glGetUniformLocation(shader.id(),"alignMode");
    isPatternEnableLocation = glGetUniformLocation(shader.id(),"isPatternEnable");
    typeBrushTipLocation = glGetUniformLocation(shader.id(),"typeBrushTip");
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
    
    glUniform1f(isSolidColorLocation, _toolSettings->brushContext.isSolidColor());
    glUniform1i(alignModeLocation, (int)_toolSettings->brushContext.getAlignMode());

    unsigned int color = _toolSettings->brushContext.getColor();
    float a = (color & 0xFF) / 255.0f;      
    float b = (color >> 8 & 0xFF) / 255.0f; 
    float g = (color >> 16 & 0xFF) / 255.0f;
    float r = (color >> 24 & 0xFF) / 255.0f;

    glUniform4f(brushColorLocation, r,g,b,a);
    glUniform1f(hardnessLocation, _toolSettings->brushContext.getHardness());
    glUniform1f(isPatternEnableLocation, _toolSettings->brushContext.isPatternEnable());
    glUniform1i(typeBrushTipLocation, (int)_toolSettings->brushContext.getTypeBrushTip());

    quad.draw();
}