#include "FramesView.h"

namespace{
    extern "C" {
        
    }
    EM_JS(void, add_frame, (string frame_id),{
        let listFrame = document.getElementById("list-frames");
        let frames = listFrame.querySelectorAll("div.frame");
        let frameElement = document.createElement("div");
        let canvas = document.createElement("canvas");

        frameElement.classList.add("frame");
        frameElement.dataset.id = frame_id;

        frameElement.append(canvas);
        if(frames.length > 0)
            frames[index-1].after(frameElement);
        else
            listFrame.append(frameElement);

        frameElement.addEventListener("click", ()=>Module.onFrameClick(frame_id));
        _update_frame_preview(frame_id, width, height, buffer);
    });
    EM_JS(void, change_active_frame, (string frame_id), {
        editor.changeActiveFrame(id);
        changeActiveFrame(editor.getActiveFrame());

        let listFrame = document.getElementById("list-frames");
        let frameElement = listFrame.querySelector(`.frame[data-id="${frame_id}"]`);
        listFrame.querySelectorAll("div.frame.active")
                 .forEach((f)=>f.classList.remove("active"));
        frameElement?.classList.toggle("active", true);
            
        /*
            listLayer.querySelectorAll(".layer")
                    .forEach((e)=>e.remove());

            let layers = activeFrame.getAllLayers();
            for(let i = 0; i < layers.size(); i++){
                addLayer(layers.get(i));
            }

            const activeLayer = activeFrame.getActiveLayer();
            Module._changeActiveLayer(activeLayer);
        */
    });
    
    EM_JS(void, remove_frame, (string frame_id), {
        let listFrame = document.getElementById("list-frames");
        let frameElement = listFrame.querySelector(`.frame[data-id="${frame_id}"]`);
        frameElement.remove();
    });
    EM_JS(void, move_frame, (string frame_id, int index), {
        const listFrame = document.getElementById("list-frames");
        let frames = listFrame.querySelectorAll("div.frame");
        let frameElement = listFrame.querySelector(`.frame[data-id="${frame_id}"]`);

        if (frameElement === frames[index] || index < 0 || index >= frames.length) {
            return;
        }

        if (frameElement.compareDocumentPosition(frames[index]) & Node.DOCUMENT_POSITION_FOLLOWING) {
            frames[index].after(frameElement);
        } else {
            frames[index].before(frameElement);
        }
        frameElement.remove();
    });
    EM_JS(void, update_frame_preview, (string frame_id, int width, int height, unsigned int* buffer), {
        const buffer8 = new Uint8ClampedArray(Module.HEAPU8.buffer, buffer, width*height*4);
        const imageData = new ImageData(buffer, width, height);

        let frameElement = listFrame.querySelector(`.frame[data-id="${frame_id}"]`);
        let frameCanvas = frameElement.querySelector(`canvas`);
        frameCanvas.width = width;
        frameCanvas.height = height;

        let ctx = frameCanvas.getContext("2d");
        ctx.clearRect(0, 0, width, height);
        ctx.putImageData(imageData, 0, 0);
    });
}

FramesView::FramesView(FramesViewModel& framesViewModel) : _framesViewModel(framesViewModel){
    emscripten_set_click_callback("#add-frame", &_framesViewModel, 0, addFrameClick); 
    emscripten_set_click_callback("#duplicate-frame", &_framesViewModel, 0, duplicateFrameClick);
    emscripten_set_click_callback("#move-down-frame", &_framesViewModel, 0, moveDownFrameClick);
    emscripten_set_click_callback("#move-up-frame", &_framesViewModel, 0, moveUpFrameClick);
    emscripten_set_click_callback("#remove-frame", &_framesViewModel, 0, removeFrameClick);
    emscripten_set_click_callback("#flip-x", &_framesViewModel, 0, flipXFrameClick);
    emscripten_set_click_callback("#flip-y", &_framesViewModel, 0, flipYFrameClick);

    
    framesViewModel.registerEvent(EDITOR_EVENT_TYPE::DRAW, drawFrame);
    framesViewModel.registerEvent(EDITOR_EVENT_TYPE::ADD_FRAME, addFrame);
    framesViewModel.registerEvent(EDITOR_EVENT_TYPE::REMOVE_FRAME, removeFrame);
    framesViewModel.registerEvent(EDITOR_EVENT_TYPE::MOVE_FRAME_TO, moveFrameTo);
    framesViewModel.registerEvent(EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME, changeActiveFrame);
}
FramesView::~FramesView(){
    emscripten_set_click_callback("#add-frame", nullptr, 0, nullptr); 
    emscripten_set_click_callback("#duplicate-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#move-down-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#move-up-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#remove-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#flip-x", nullptr, 0, nullptr);
    emscripten_set_click_callback("#flip-y", nullptr, 0, nullptr);
}
void FramesView::render(){
    // for(Frame* frame : _editor.getAllFrames()){
    //     add_frame(frame->getID().toString());
    // }
}
void FramesView::addFrame(EditorEvent event){
    add_frame(event.frame_id.toString());
}
void FramesView::removeFrame(EditorEvent event){
    remove_frame(event.frame_id.toString());
}
void FramesView::moveFrameTo(EditorEvent event){
    move_frame(event.frame_id.toString(), event.index);
}
void FramesView::changeActiveFrame(EditorEvent event){
    change_active_frame(event.frame_id.toString());
}
void FramesView::drawFrame(EditorEvent event){
    printf("draw_alo\n");
    // update_frame_preview(event.frame_id);
}

void FramesView::onFrameClick(string frameId){
    // Guid id(frameId);
    // _framesViewModel.onChangeActiveFrame(id);
}
// void FramesView::onAddFrameClick(){
//     _framesViewModel.onAddFrame();
// }
// void FramesView::onRemoveFrameClick(){
//     _framesViewModel.onChangeActiveFrame();
// }
// void FramesView::onMoveUpFrameClick(){
//     _framesViewModel.onMoveUpFrame(id);
// }
// void FramesView::onMoveDownFrameClick(){
//     _framesViewModel.onMoveDownFrame(id);
// }
// void FramesView::onDuplicateFrameClick(){
//     _framesViewModel.onDuplicateFrame(id);
// }

EM_BOOL FramesView::addFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onAddFrame();

    return EM_FALSE;
}
EM_BOOL FramesView::duplicateFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onDuplicateFrame();

    return EM_FALSE;
}
EM_BOOL FramesView::moveDownFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onMoveDownFrame();

    return EM_FALSE;
}
EM_BOOL FramesView::moveUpFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onMoveUpFrame();

    return EM_FALSE;
}
EM_BOOL FramesView::removeFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onRemoveFrame();

    return EM_FALSE;
}
EM_BOOL FramesView::flipXFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onFlipXFrame();

    return EM_FALSE;
}
EM_BOOL FramesView::flipYFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    FramesViewModel* frameView = static_cast<FramesViewModel*>(userData);
    frameView->onFlipYFrame();

    return EM_FALSE;
}


#include <emscripten/bind.h>
using namespace emscripten;
EMSCRIPTEN_BINDINGS(app)
{
    emscripten::function("onFrameClick", &FramesView::onFrameClick);
}