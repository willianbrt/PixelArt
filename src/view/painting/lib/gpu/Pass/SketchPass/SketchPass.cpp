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
            float time;
        };

        uniform vec2 texSize;
        uniform vec2 gridDivisions;
        uniform vec4 lightColor;
        uniform vec4 darkColor;

        out vec4 fragColor;

        void main()
        {
            vec2 transformedUV = pixel / texSize;
            
            if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y < 0.0 || transformedUV.y > 1.0) discard;
            
            vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
            vec2 thickness = 1.0 / (zoom * texSize);
            transformedUV = fract(transformedUV * repeat);

            vec4 textureColor = texture(tex, transformedUV).abgr;
            color = mix(color, textureColor, textureColor.a);

            vec2 divisions = min(gridDivisions, texSize);
            vec2 cellSize = texSize / divisions;
            vec2 cellCoord = floor(pixel / cellSize);

            float checker = mod(floor(cellCoord.x) + floor(cellCoord.y), 2.0);
            vec4 background = mix(lightColor, darkColor, checker);
            color = mix(background, textureColor, textureColor.a);

            vec2 gridPos = mod(transformedUV, cellSize / texSize);

            float line = step(gridPos.x, thickness.x) + step(gridPos.y, thickness.y);
            line = clamp(line, 0.0, 1.0);

            fragColor = mix(color, vec4(0.0, 0.0, 0.0, 1.0), line);
        })";

    quad.create();
    shader.create(fs);
    shader.use();

    globalUBO.create();
    glUniformBlockBinding(shader.id(), glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);

    texture = Texture(&shader, "tex");

    texSizeLocation = glGetUniformLocation(shader.id(),"texSize");
    gridDivisionsLocation = glGetUniformLocation(shader.id(),"gridDivisions");
    lightColorLocation = glGetUniformLocation(shader.id(),"lightColor");
    darkColorLocation = glGetUniformLocation(shader.id(),"darkColor");
}
SketchPass::~SketchPass(){}

void SketchPass::render(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    _surface = editor->getSurface();
    if(!_surface) return;

    if(!texture.isInitialized()) texture.init(_surface);

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

    CanvasSettings* canvasSettings = editor->getCanvasSettings();
    Point panning = canvasSettings->getSketchPosition();
    Point cursorLocation = canvasSettings->cursorToCanvas(_viewport->cursorX, _viewport->cursorY);

    GlobalData data;
    data.resolution[0] = _viewport->width;
    data.resolution[1] = _viewport->height;
    data.cursor[0] = _viewport->cursorX;
    data.cursor[1] = _viewport->cursorY;
    data.cursorLocation[0] = cursorLocation.x;
    data.cursorLocation[1] = cursorLocation.y;
    data.pan[0] = panning.x;
    data.pan[1] = panning.y;
    data.repeat[0] = canvasSettings->getTilesX();
    data.repeat[1] = canvasSettings->getTilesY();
    data.zoom[0] = canvasSettings->getScale();
    data.zoom[1] = canvasSettings->getScale();
    data.time = glfwGetTime();

    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO.id());
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);

    glUniform2f(gridDivisionsLocation, canvasSettings->getGridDivisionsX(), canvasSettings->getGridDivisionsY());
    
    glUniform2f(texSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform4f(lightColorLocation, 1,1,1,1);
    glUniform4f(darkColorLocation, 1.0f,1.0f,1.0f,0.3f);

    quad.draw();
}