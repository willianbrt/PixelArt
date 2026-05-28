#include "./CursorPass.h"

CursorPass::CursorPass(){
    glUseProgram(programHover);
    posH = glGetAttribLocation(programHover,"position");
    texture = glGetUniformLocation(programHover,"tex");
    resolutionLocationH = glGetUniformLocation(programHover,"resolution");
    positionLocationH = glGetUniformLocation(programHover,"pan");
    scaleLocationH = glGetUniformLocation(programHover,"zoom");
    texSizeLocationH = glGetUniformLocation(programHover,"texSize");
    cursorLocation = glGetUniformLocation(programHover,"cursor");
    brushSizeLocation = glGetUniformLocation(programHover,"brushSize");
}
CursorPass::~CursorPass(){}

void CursorPass::init(){
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
}
void CursorPass::draw(){
    glUseProgram(programHover);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture, 0);

    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(posH);
    glVertexAttribPointer(posH, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // CanvasSettings* canvasSettings = viewport->getCanvasSettings();
    // glUniform2f(resolutionLocationH, viewport->getWidth(), viewport->getHeight());
    // glUniform2f(positionLocationH,  canvasSettings->getSketchPosition().x, canvasSettings->getSketchPosition().y);
    // glUniform1f(scaleLocationH, canvasSettings->getScale());
    // glUniform2f(texSizeLocationH, surface->getWidth(), surface->getHeight());

    Point cursor = viewport->getCursor();
    Point cursorCanvas = viewport->cursorToCanvas(cursor.x, cursor.y);

    glUniform2f(cursorLocation, cursorCanvas.x, cursorCanvas.y);
    glUniform2f(brushSizeLocation, hover->pattern->width, hover->pattern->height);
    
    quad.draw();
}
void CursorPass::upload(Bounding area){
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