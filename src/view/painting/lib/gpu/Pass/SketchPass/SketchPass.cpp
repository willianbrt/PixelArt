#include "./SketchPass.h"

SketchPass::SketchPass(){
    
}
SketchPass::SketchPass(EditorManager* manager, ViewportContext* viewport){
    _manager = manager;
    _viewport = viewport;

    
    const char* fs =
    R"(
    precision highp float;
    uniform sampler2D tex;
    varying vec2 uv;
    uniform vec2 texSize;
    uniform vec2 gridDivisions;
    uniform vec4 lightColor;
    uniform vec4 darkColor;

    uniform vec2 brushSize;
    
    uniform vec2 cursor;
    uniform float time;
    uniform vec2 resolution;
    uniform vec2 pan;
    uniform float zoom;
    uniform vec2 repeat;

    void main(){
       vec4 color = vec4(1.0,0.0,0.0,1.0);
       vec2 pixel = (uv - pan) / (zoom + repeat-1.0);
       vec2 transformedUV = pixel / texSize;
       vec2 thickness = 1.0/(zoom*texSize);
       if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y < 0.0 || transformedUV.y > 1.0) discard;

       transformedUV = fract(transformedUV * repeat);
       vec4 textureColor = texture2D(tex, transformedUV).abgr;
       color = mix(color, textureColor, textureColor.a);

       vec2 divisions = min(gridDivisions, texSize);
       vec2 cellSize = texSize/divisions;
       vec2 cellCoord = floor(pixel / cellSize);

       float checker = mod(floor(cellCoord.x) + floor(cellCoord.y), 2.0);
       vec4 background = mix(lightColor, darkColor, checker);
       color = mix(background, textureColor, textureColor.a);

       vec2 gridPos = mod(transformedUV, cellSize/texSize);
       float line = step(gridPos.x, thickness.x) + step(gridPos.y, thickness.y);
       line = clamp(line, 0.0, 1.0);

       color = mix(color, vec4(0.0,0.0,0.0,1.0), line);

       gl_FragColor = color;
    })";

    quad.create();
    shader.create(fs);
    shader.use();

    resolutionLocation = glGetUniformLocation(shader.id(),"resolution");
    positionLocation = glGetUniformLocation(shader.id(),"pan");
    scaleLocation = glGetUniformLocation(shader.id(),"zoom");
    

    pos = glGetAttribLocation(shader.id(),"position");
    texture = glGetUniformLocation(shader.id(),"tex");
    texSizeLocation = glGetUniformLocation(shader.id(),"texSize");
    repeatLocation = glGetUniformLocation(shader.id(),"repeat");
    gridDivisionsLocation = glGetUniformLocation(shader.id(),"gridDivisions");
    lightColorLocation = glGetUniformLocation(shader.id(),"lightColor");
    darkColorLocation = glGetUniformLocation(shader.id(),"darkColor");
}
SketchPass::~SketchPass(){}

void SketchPass::init(){
    if (glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _surface->getWidth(),
        _surface->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _surface->getBuffer()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void SketchPass::draw(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    _surface = editor->getSurface();
    if(!_surface) return;
    if(!glIsTexture(texture)) init();
    
    Bounding area = editor->preview()->getDirtyArea();
    if(area.start.x != INT_MAX
    && area.start.y != INT_MAX
    && area.end.x != INT_MIN
    && area.end.y != INT_MIN){
        editor->compose();
        upload(area);
    }
        
    shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.id(),"tex"), 0);
    
    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniform2f(resolutionLocation, _viewport->width, _viewport->height);
    glUniform2f(cursorWorldLocation, _viewport->cursorX, _viewport->cursorY);

    CanvasSettings* canvasSettings = editor->getCanvasSettings();
    Point panning = canvasSettings->getSketchPosition();
    glUniform2f(positionLocation, panning.x, panning.y);
    glUniform1f(scaleLocation, canvasSettings->getScale());
    glUniform2f(repeatLocation, canvasSettings->getTilesX(), canvasSettings->getTilesY());
    glUniform2f(gridDivisionsLocation, canvasSettings->getGridDivisionsX(), canvasSettings->getGridDivisionsY());

    // printf("v:%i,%i - ",_viewport->width, _viewport->height);
    // printf("c:%i,%i - ",_viewport->cursorX, _viewport->cursorY);
    // printf("p:%i,%i - ",panning.x, panning.y);
    // printf("z:%f - ",canvasSettings->getScale());
    // printf("t:%i,%i - ",canvasSettings->getTilesX(), canvasSettings->getTilesY());
    // printf("g:%i,%i - ",canvasSettings->getGridDivisionsX(), canvasSettings->getGridDivisionsY());
    // printf("s:%i,%i\n",_surface->getWidth(), _surface->getHeight());
    
    glUniform2f(texSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform4f(lightColorLocation, 1,1,1,1);
    glUniform4f(darkColorLocation, 1.0f,1.0f,1.0f,0.3f);

    quad.draw();
}

void SketchPass::upload(Bounding area){
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, _surface->getWidth());
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, area.start.x);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, area.start.y);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        area.start.x,
        area.start.y,
        area.getWidth(),
        area.getHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _surface->getBuffer()
    );

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}