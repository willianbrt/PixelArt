#include "./SketchPass.h"

SketchPass::SketchPass(){
    
}
SketchPass::SketchPass(EditorManager* manager, ViewportContext* viewport) {
    _manager = manager;
    _viewport = viewport;
    const char* fs =
        R"(#version 300 es
        precision highp float;

        uniform sampler2D tex;

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

        uniform vec2 texSize;
        uniform vec4 lightColor;
        uniform vec4 darkColor;

        out vec4 fragColor;

        void main()
        {
            vec2 pixelCoord = floor(pixel);
            vec2 transformedUV = fract(pixelCoord / texSize);
            vec2 thickness = 1.0 / zoom;

            vec4 color = vec4(0.0);

            vec4 textureColor = texture(tex, transformedUV).abgr;
            vec2 currentTile = floor(pixelCoord / texSize);
            if(currentTile != (repeat-1.0)/2.0){
                textureColor = vec4(textureColor.rgb, textureColor.a * 0.5);
            }

            vec2 cellCoord = floor((pixel-gridPan) / gridDivisions);
            float checker = mod(cellCoord.x + cellCoord.y, 2.0);
            vec4 background = mix(lightColor, darkColor, checker);
            color = mix(background, textureColor, textureColor.a);

            vec2 maxBounds = repeat * texSize;
            bool outEditor = pixel.x < 0.0 || pixel.y < 0.0 || pixel.x >= maxBounds.x || pixel.y >= maxBounds.y;
                        
            bool isBorderOutside = pixel.x < -thickness.x || pixel.y < -thickness.y || pixel.x >= maxBounds.x + thickness.x || pixel.y >= maxBounds.y + thickness.y;
            if(isBorderOutside) discard;
            
            bool border = !isBorderOutside && outEditor;
            
            fragColor = border ? vec4(0.87, 0.411, 0.0, 1.0) : color;
            

        })";

    quad.create();
    shader.create(fs);
    shader.use();

    globalUBO.create();
    glUniformBlockBinding(shader.id(), glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);

    texture = Texture(1, &shader, "tex");

    texSizeLocation = glGetUniformLocation(shader.id(),"texSize");
    lightColorLocation = glGetUniformLocation(shader.id(),"lightColor");
    darkColorLocation = glGetUniformLocation(shader.id(),"darkColor");
}
SketchPass::~SketchPass(){}

void SketchPass::render(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    _surface = editor->getSurface();
    if(!_surface) return;

    texture.init(_surface);

    DirtyManager * dirtyManager = editor->getDirtyManager();
    if(dirtyManager->hasDirty()){
        editor->compose(dirtyManager->dirty());
        texture.upload(dirtyManager->dirty());
        dirtyManager->validade();
    }
    
    shader.use();
    
    texture.active();
    
    quad.bind();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);

    SymmetryContext* symmetryContext = editor->getSymmetryContext();
    CanvasSettings* canvasSettings = editor->getCanvasSettings();
    Point cursorLocation = canvasSettings->cursorToCanvas(_viewport->cursorX, _viewport->cursorY);

    GlobalData data;
    data.resolution[0] = _viewport->width;
    data.resolution[1] = _viewport->height;
    data.cursor[0] = _viewport->cursorX;
    data.cursor[1] = _viewport->cursorY;
    data.cursorLocation[0] = cursorLocation.x;
    data.cursorLocation[1] = cursorLocation.y;
    data.pan[0] = canvasSettings->canvasTransform.pan.x;
    data.pan[1] = canvasSettings->canvasTransform.pan.y;
    data.repeat[0] = canvasSettings->tilingContext.isTilingX ? TilingContext::N_TILE_X : 1;
    data.repeat[1] = canvasSettings->tilingContext.isTilingY ? TilingContext::N_TILE_Y : 1;
    data.zoom[0] = canvasSettings->canvasTransform.scale;
    data.zoom[1] = canvasSettings->canvasTransform.scale;
    data.gridDivisions[0] = canvasSettings->gridContext.divisionsX;
    data.gridDivisions[1] = canvasSettings->gridContext.divisionsY;
    data.gridPan[0] = ((editor->getWidth() %canvasSettings->gridContext.divisionsX) >> 1) + editor->getWidth() *((int)data.repeat[0]>>1);
    data.gridPan[1] = ((editor->getHeight()%canvasSettings->gridContext.divisionsY) >> 1) + editor->getHeight()*((int)data.repeat[1]>>1);
    data.time = glfwGetTime();

    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO.id());
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
    
    glUniform2f(texSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform4f(lightColorLocation, 1,1,1,1);
    glUniform4f(darkColorLocation, 1.0f,1.0f,1.0f,0.3f);

    quad.draw();
}