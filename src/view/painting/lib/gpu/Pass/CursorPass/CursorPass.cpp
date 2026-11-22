#include "./CursorPass.h"

CursorPass::CursorPass(EditorManager* manager, ToolManager* toolManager, ToolSettings* toolSettings, ViewportContext* viewport){
    _manager = manager;
    _toolManager = toolManager;
    _toolSettings = toolSettings;
    _viewport = viewport;
    
    const char* fs =
        R"(#version 300 es
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
            vec2 gridDivisions;
            vec2 gridPan;
            float time;
        };
        
        uniform vec2 scale;
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
            vec2 pixelCoord = floor(pixel);
            vec2 transformedUV = pixelCoord / editorSize;
            if(transformedUV.x < 0.0 || transformedUV.x >= repeat.x || transformedUV.y < 0.0 || transformedUV.y >= repeat.y) discard;

            vec2 shapeSize = floor(texShapeSize * scale);
            vec2 pixelShape = pixelCoord - (cursorLocation - shapeSize * 0.5);
            vec2 hoverUV = pixelShape / shapeSize;

            float shapeMask = texture(texShape, hoverUV).r;
            
            vec3 c = color.rgb;
            float patternMask = 1.0;
            if(isPatternEnable) {
                vec4 patternPixel;
                if(texPatternSize.x > 0.0 && texPatternSize.y > 0.0 ){
                    patternPixel = getPatternPixel(texPattern, texPatternSize);
                } else {
                    shapeMask = 1.0;
                    patternPixel = getPatternPixel(texShape, texShapeSize);
                }
                patternMask = patternPixel.r;

                if(!isSolidColor) {
                    c = patternPixel.abg;
                }
            } else{
                if(!isSolidColor && typeBrushTip != 0){
                    c = texture(texShape, hoverUV).abg;
                }
            }

            if (all(greaterThanEqual(floor(pixelShape), vec2(0.0))) &&
                all(lessThan(floor(pixelShape), shapeSize)))
                fragColor = vec4(c, shapeMask*patternMask*hardness);


                
            vec2 maxBounds = repeat * editorSize;
            bool isntCanvas = pixel.x < 0.0 || pixel.y < 0.0 || pixel.x >= maxBounds.x || pixel.y >= maxBounds.y;
            
            vec2 thicknessTile = vec2(1.0);
            vec2 gridPos = mod(uv* resolution-pan , editorSize*zoom);
            
            float outlineX = step(gridPos.x, thicknessTile.x);
            float outlineY = step(gridPos.y, thicknessTile.y);

            float tileGrid = max(
            outlineX * float(pixelCoord.x > 0.0) * float(pixelCoord.x < maxBounds.x),
            outlineY * float(pixelCoord.y > 0.0) * float(pixelCoord.y < maxBounds.y)
            );

            fragColor = mix(fragColor, vec4(0.87, 0.411, 0.0, 1.0), tileGrid);
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

    ISurface* shape = _toolSettings->drawingContext.shape->getMask();
    
    if(_toolSettings->drawingContext.pattern != nullptr){
        if(_pattern != _toolSettings->drawingContext.pattern->getMask()){
            _pattern = _toolSettings->drawingContext.pattern->getMask();
            texturePattern.init(_pattern);
            if(!textureShape.isInitialized()) return;
        }
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
    
    Transformation transform = _toolSettings->drawingContext.shape->getTransformation();
    const PointF* scale = transform.getScale();

    glUniform2f(glGetUniformLocation(shader.id(),"scale"), scale->x, scale->y);
    glUniform2f(editorSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform2f(texShapeSizeLocationH, _shape == nullptr ? 0 : _shape->getWidth(), _shape == nullptr ? 0 :_shape->getHeight());
    glUniform2f(texPatternSizeLocationH, _pattern == nullptr ? 0 : _pattern->getWidth(), _pattern == nullptr ? 0 :_pattern->getHeight());

    glUniform2f(brushSizeLocation, (float)_toolSettings->drawingContext.shape->getSize(), (float)_toolSettings->drawingContext.shape->getSize());
    
    glUniform1f(isSolidColorLocation, _toolSettings->drawingContext.isSolidColor);
    glUniform1i(alignModeLocation, (int)_toolSettings->drawingContext.alignMode);


    glUniform4f(brushColorLocation, _toolSettings->drawingContext.color.red, _toolSettings->drawingContext.color.green, _toolSettings->drawingContext.color.blue, _toolSettings->drawingContext.hardness);
    glUniform1f(hardnessLocation, _toolSettings->drawingContext.hardness);
    glUniform1f(isPatternEnableLocation, _toolSettings->drawingContext.enablePattern);
    glUniform1i(typeBrushTipLocation, (int)_toolSettings->drawingContext.shape->getType());

    quad.draw();
}