#include "RendererPipeline.h"

RendererPipeline::RendererPipeline(){
}
void RendererPipeline::registerPass(IRenderPass* pass){
    passes.push_back(pass);
}

void RendererPipeline::render(){
    for(int i = 0; i < passes.size(); i++){
        passes[i]->draw();
    }
    
    // if(!_editorManager) { return; } 

    // Editor* editor = _editorManager.get()->getActiveEditor();

    // if (editor != _activeEditor) { 
    //     _activeEditor = editor;
    
    //     _activeEditor->compose();
    //     _renderer->init(_activeEditor->getSurface(), _viewport);

    //     Bounding dirtArea = {
    //         Point(0,0),
    //         Point(_activeEditor->getWidth(), _activeEditor->getHeight())
    //     };
    //     _renderer->uploadSurface(dirtArea, _activeEditor->getSurface());
    // }else{
    //     Preview* preview = _activeEditor->preview();
    //     // Bounding dirtArea = preview->getDirtyArea();
        
    //     Bounding dirtArea = {
    //         Point(0,0),
    //         Point(_activeEditor->getWidth(), _activeEditor->getHeight())
    //     };
    //     if(dirtArea.getWidth() > 0 && dirtArea.getHeight() > 0){
    //         _activeEditor->compose(dirtArea);
    //         _renderer->uploadSurface(dirtArea, _activeEditor->getSurface());
    //     }
    // }
    
    // _renderer->draw(_activeEditor->getSurface(), _viewport);

    // IPressedStrategy* tool = _toolManager->getToolPressed();
    // if(!tool) return;

    // CursorContext* cursorContext = tool->getCursorContext();
    // if(!cursorContext) return;
    // if(_cursorContext->pattern->height != cursorContext->pattern->height || _cursorContext->pattern->width != cursorContext->pattern->width){
    //     _cursorContext = cursorContext;
    //     _renderer->initCursorHover(_cursorContext);
    // }

    // SelectContext* select = _activeEditor->getSelectContext();
    // // if(!!select->data){
    //     _renderer->initSelect(_activeEditor);
    //     _renderer->uploadSelect(_activeEditor);
    //     _renderer->drawSelect(_activeEditor->getSurface(), _activeEditor, _viewport);
    // // }

    // if(cursorContext->enable){
    //     _renderer->drawCursorHover(_activeEditor->getSurface(), _cursorContext, _viewport);
    // }

}
