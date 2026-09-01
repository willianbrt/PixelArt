#include "Editor.h"

Editor::Editor(int width, int height) {
    _sketch = new Surface(width, height);
    _preview = new Preview(width, height);
    
    _drawingSession = new DrawingSession(_preview, &dirtyManager, &_symmetry);

    _canvasSettings = new CanvasSettings();
    _select = new SelectContext();
    _canvasSettings->setScale(15.0f);
    _canvasSettings->setGridDivisionsX(32);
    _canvasSettings->setGridDivisionsY(32);

}
Editor::~Editor(){
    frames.clear();
    activeFrame = nullptr;
    free(_preview);
}

void Editor::registerEvent(IEditorObserver* observer){
    observers.push_back(observer);
}

Preview* Editor::preview(){ return _preview; }
DrawingSession* Editor::getDrawingSession(){ return _drawingSession; }

void Editor::compose(){
    Bounding boundingSketch = Bounding(Point(0,0), Point(_sketch->getWidth(), _sketch->getHeight()));

    compose(boundingSketch);
}
void Editor::compose(Bounding area){
    if(area.start.x > _sketch->getWidth() || area.start.y > _sketch->getHeight()) return;
    if(area.start.x < 0 || area.start.y < 0) return;

    size_t activeFrameIndex = getFrameIndex(activeFrame->getID());

    Layer* drawingLayer = _preview ? _preview->getTarget() : nullptr;
    size_t drawingLayerIndex = activeFrame->getLayerIndex(drawingLayer->getID());


    float opacity = 0.3;
    for(int y = area.start.y; y < area.end.y; y++){
        int index = y * _sketch->getWidth() + area.start.x;
        
        for(int x = area.start.x; x < area.end.x; x++){
            // previous frame
            unsigned int colorHex = activeFrameIndex > 0 ? frames[activeFrameIndex - 1].get()->getPixel(index) : 0;
            GraphicsEngine::setOpacity(colorHex, opacity);

            // active frame
            colorHex = GraphicsEngine::blendColors(colorHex, activeFrame->getPixel(index, 0, drawingLayerIndex));
            if(drawingLayer){
                colorHex = GraphicsEngine::blendColors(colorHex, _preview->getPixel(index));
            } else {
                colorHex = GraphicsEngine::blendColors(colorHex, activeFrame->getPixel(index, drawingLayerIndex, drawingLayerIndex+1));
            }
            colorHex = GraphicsEngine::blendColors(colorHex, activeFrame->getPixel(index, drawingLayerIndex+1, activeFrame->getLayersLength()));
            
            // view
            _sketch->putPixel(index, colorHex);
            
            index++;
        }
    }
    
}

void Editor::resize(int width, int height){
}

Surface* Editor::getSurface(){ return _sketch; }
unsigned int* Editor::getBuffer(){ return _sketch->getBuffer(); }
int Editor::getWidth(){ return _sketch->getWidth(); }
int Editor::getHeight(){ return _sketch->getHeight(); }


void Editor::addFrame(unique_ptr<Frame> frame, size_t index){
    frames.insert(frames.begin() + index,  std::move(frame));

    Frame* framePtr = frames[index].get();
    for (auto* obs : observers) {
        obs->onAddFrame(framePtr, index);
    }
    changeActiveFrame(framePtr->getID());
}
unique_ptr<Frame> Editor::removeFrame(size_t index){
    if(index < 0 || index >= frames.size()) return nullptr;
    
    for (auto* obs : observers) {
        obs->onRemoveFrame(frames[index]->getID());
    }
    unique_ptr<Frame> removedFrame = std::move(frames[index]);
    frames.erase(frames.begin() + index);
    
    dirtyManager.markDirty({{0,0},{_sketch->getWidth(), _sketch->getHeight()}});

    return removedFrame;
}
void Editor::changeActiveFrame(Guid id){
    activeFrame = getFrameByID(id);

    if(_preview) free(_preview);
    _preview = new Preview(_sketch->getWidth(), _sketch->getHeight());
    _preview->setTarget(activeFrame->getActiveLayer());

    dirtyManager.markDirty({{0,0},{_sketch->getWidth(), _sketch->getHeight()}});

    for (auto* obs : observers) {
        obs->onChangeActiveFrame(id);
    }
}
void Editor::bringFrameTo(Guid id, size_t toIndex){
    auto from = getIteratorFrameByID(id);

    if (from == frames.end()) return;

    size_t fromIndex = std::distance(frames.begin(), from);
    
    if (fromIndex == toIndex || fromIndex >= frames.size() || toIndex >= frames.size()) return;
    
    if (fromIndex < toIndex) {
        std::rotate(frames.begin() + fromIndex, frames.begin() + fromIndex + 1, frames.begin() + toIndex + 1);
    } else {
        std::rotate(frames.begin() + toIndex, frames.begin() + fromIndex, frames.begin() + fromIndex + 1);
    }

    dirtyManager.markDirty({{0,0},{_sketch->getWidth(), _sketch->getHeight()}});
    for (auto* obs : observers) {
        obs->onMoveFrameTo(id, toIndex);
    }
}

size_t Editor::getFramesLength() {
    return frames.size();
}
Frame* Editor::getFrameByIndex(size_t index) {
    return frames[index].get();
}
Frame* Editor::getFrameByID(Guid id){
    auto it = getIteratorFrameByID(id);
    return (it != frames.end()) ? it->get() : nullptr;
}
size_t Editor::getFrameIndex(Guid id){
    auto it = getIteratorFrameByID(id);
    if (it != frames.end())
        return std::distance(frames.begin(), it);
    return -1;
}
std::vector<unique_ptr<Frame>>::iterator Editor::getIteratorFrameByID(Guid id){
    string idStr = id.toString();
    return std::find_if(frames.begin(), frames.end(), [&idStr](const std::unique_ptr<Frame>& f){
        return f.get()->getID().toString() == idStr;
    });
}
Frame* Editor::getActiveFrame(){
    return activeFrame;
}

CanvasSettings* Editor::getCanvasSettings(){
    return _canvasSettings;
}
SelectContext* Editor::getSelectContext(){
    return _select;
}
SymmetryContext* Editor::getSymmetryContext(){
    return &_symmetry;
}
Point Editor::getCanvasSize(){
    return {
        _canvasSettings->getTilesX() * _sketch->getWidth(),
        _canvasSettings->getTilesY() * _sketch->getHeight()
    };
}
DirtyManager* Editor::getDirtyManager(){
    return &dirtyManager;
}
using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    enum_<EDITOR_EVENT_TYPE>("EDITOR_EVENT_TYPE")
        .value("DRAW", EDITOR_EVENT_TYPE::DRAW)
        .value("ADD_FRAME", EDITOR_EVENT_TYPE::ADD_FRAME)
        .value("REMOVE_FRAME", EDITOR_EVENT_TYPE::REMOVE_FRAME)
        .value("MOVE_FRAME_TO", EDITOR_EVENT_TYPE::MOVE_FRAME_TO)
        .value("CHANGE_ACTIVE_FRAME", EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME);
};