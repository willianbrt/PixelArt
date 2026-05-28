#include "./SelectPass.h"

SelectPass::SelectPass(){
    
    glUseProgram(programSelect);
    posSelect = glGetAttribLocation(programSelect,"position");
    texture = glGetUniformLocation(programSelect,"tex");
    resolutionLocationSelect = glGetUniformLocation(programSelect,"resolution");
    positionLocationSelect = glGetUniformLocation(programSelect,"pan");
    scaleLocationSelect = glGetUniformLocation(programSelect,"zoom");
    texSizeLocationSelect = glGetUniformLocation(programSelect,"texSize");
    cursorWorldLocation = glGetUniformLocation(programSelect,"cursorWorld");
    selectSizeLocation = glGetUniformLocation(programSelect,"selectSize");
}
SelectPass::~SelectPass(){}

void SelectPass::init(){
    if(initializedSelect) return;
    initializedSelect = true;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        editor->getWidth(),
        editor->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        editor->preview()->getBuffer()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void SelectPass::draw(){
    if(!initializedSelect) return;
    
    glUseProgram(programSelect);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture, 0);

    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(posSelect);
    glVertexAttribPointer(posSelect, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // CanvasSettings* canvasSettings = viewport->getCanvasSettings();
    // glUniform2f(resolutionLocationSelect, viewport->getWidth(), viewport->getHeight());
    // glUniform2f(positionLocationSelect,  canvasSettings->getSketchPosition().x, canvasSettings->getSketchPosition().y);
    // glUniform1f(scaleLocationSelect, canvasSettings->getScale());
    // glUniform2f(texSizeLocationSelect, surface->getWidth(), surface->getHeight());

    Point cursor = viewport->getCursor();
    Point cursorCanvas = viewport->cursorToCanvas(cursor.x, cursor.y);

    glUniform2f(cursorLocation, cursorCanvas.x, cursorCanvas.y);
    glUniform2f(cursorWorldLocation, cursor.x, cursor.y);
    // glUniform2f(selectSizeLocation, editor->getSelectContext()->data->getWidth(), editor->getSelectContext()->data->getHeight());
    glUniform2f(selectSizeLocation, surface->getWidth(), surface->getHeight());
    // glUniform2f(cursorLocation, cursor.x, cursor.y);
    
    SelectContext* select = editor->getSelectContext();    
    glUniform2fv(glGetUniformLocation(programSelect,"resizeHandle"), 4, select->getAllHandle(viewport).data());
    glUniform2fv(glGetUniformLocation(programSelect,"rotateHandle"), 4, select->getAllRotateHandle(viewport).data());

    float time = glfwGetTime();
    glUniform1f(glGetUniformLocation(programSelect,"time"), time);

    quad.draw();
}
void SelectPass::upload(Bounding area){
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, editor->getWidth());
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        editor->getWidth(),
        editor->getHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        editor->preview()->getBuffer()
    );

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}