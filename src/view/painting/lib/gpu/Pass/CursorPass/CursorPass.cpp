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
        uniform vec2 maskAlign;
        
        out vec4 fragColor;
        void main(){
            vec2 transformedUV = pixel / editorSize;
            if(transformedUV.x < 0.0 || transformedUV.x > repeat.x || transformedUV.y < 0.0 || transformedUV.y > repeat.y) discard;

            vec2 pixelShape = (pixel-cursorLocation) + floor(texShapeSize*0.5);
            vec2 hoverUV = pixelShape / texShapeSize;
            if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;
            

            vec2 patternUV;
            if(alignMode == 0){
                patternUV = hoverUV;
            } else if(alignMode == 1) {
                patternUV = fract((pixel- (cursorLocation - floor(texShapeSize*0.5))) / texPatternSize);
            } else {
                patternUV = fract((pixel-maskAlign) / texPatternSize);
            }

            if(fillMode == 0){
                fragColor = vec4(color.r,color.g,color.b,texture(texShape, patternUV).r*hardness); // SOLID COLOR SHAPE
            } else if(fillMode == 1) {
                fragColor = vec4(color.r,color.g,color.b,texture(texPattern, patternUV).r*hardness);  // SOLID COLOR PATTERN
            } else {
                // fragColor = mix(vec4(0.0), texture(texShape, patternUV), hardness); // COLOR PATTERN
                fragColor = vec4(texture(texPattern, patternUV).a,texture(texPattern, patternUV).g,texture(texPattern, patternUV).b,texture(texPattern, patternUV).r*hardness);
            }


            // // vec2 patternUV = fract(pixel / texPatternSize);
            
            // vec2 patternUV = fract((pixel-maskAlign) / texPatternSize);
            // fragColor = vec4(color.r,color.g,color.b,texture(texShape, patternUV).r*hardness);
        })";
    
    
    quad.create();
    shader.create(fs);
    shader.use();

    glUniformBlockBinding(shader.id(),  glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);
    editorSizeLocation = glGetUniformLocation(shader.id(),"editorSize");
    
    texturePattern = Texture(2, &shader, "texPattern");
    texPatternSizeLocationH = glGetUniformLocation(shader.id(),"texPatternSize");
    
    textureShape = Texture(2, &shader, "texShape");
    texShapeSizeLocationH = glGetUniformLocation(shader.id(),"texShapeSize");


    brushSizeLocation = glGetUniformLocation(shader.id(),"brushSize");
    brushColorLocation = glGetUniformLocation(shader.id(),"color");
    hardnessLocation = glGetUniformLocation(shader.id(),"hardness");
    fillModeLocation = glGetUniformLocation(shader.id(),"fillMode");
    alignModeLocation = glGetUniformLocation(shader.id(),"alignMode");
    maskAlignLocation = glGetUniformLocation(shader.id(),"maskAlign");
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

    ISurface* pattern = cursor->pattern;
    textureShape.init(pattern);
    if(!textureShape.isInitialized()) return;

    ISurface* shape = _toolSettings->brushContext.selectedPattern;
    texturePattern.init(shape);
    if(!texturePattern.isInitialized()) return;

    shader.use();
    textureShape.active();
    texturePattern.active();
    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUniform2f(editorSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform2f(texShapeSizeLocationH, shape->getWidth(), shape->getHeight());
    glUniform2f(texPatternSizeLocationH, pattern->getWidth(), pattern->getHeight());

    glUniform2f(brushSizeLocation, (float)_toolSettings->drawingContext.size, (float)_toolSettings->drawingContext.size);
    
    // CanvasSettings* canvasSettings = editor->getCanvasSettings();
    // Point maskAlign =  canvasSettings->cursorToCanvas(_viewport->cursorX, _viewport->cursorY);
    // glUniform2f(maskAlignLocation, (float)maskAlign.x-(pattern->getWidth()>>1), (float)maskAlign.y-(pattern->getHeight()>>1));

    Point maskAlign =  {0,0};
    glUniform2f(maskAlignLocation, (float)maskAlign.x, (float)maskAlign.y);

    _toolSettings->brushContext.fillMode = FILL_MODE::STAMP;
    _toolSettings->brushContext.alignMode = ALIGN_MODE::BRUSHING;
    glUniform1i(fillModeLocation,  (int)_toolSettings->brushContext.fillMode);
    glUniform1i(alignModeLocation, (int)_toolSettings->brushContext.alignMode);

    float a = (_toolSettings->drawingContext.color & 0xFF) / 255.0f;      
    float b = (_toolSettings->drawingContext.color >> 8 & 0xFF) / 255.0f; 
    float g = (_toolSettings->drawingContext.color >> 16 & 0xFF) / 255.0f;
    float r = (_toolSettings->drawingContext.color >> 24 & 0xFF) / 255.0f;
    glUniform4f(brushColorLocation, r,g,b,a);
    glUniform1f(hardnessLocation, _toolSettings->drawingContext.hardness);

    quad.draw();
}