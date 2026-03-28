#include "Editor.h"

Editor::Editor(int width, int height) {
    _sketch = new Surface(width, height);
    _preview = new Preview(width, height);
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

void Editor::draw(IGraphic& graphic){
    activeFrame->draw(graphic);
    compose();
}


void Editor::compose(){
    Bounding boundingSketch = Bounding(Point(0,0), Point(_sketch->getWidth(), _sketch->getHeight()));

    compose(boundingSketch);
}
void Editor::compose(Bounding area){
    if(area.start.x > _sketch->getWidth() || area.start.y > _sketch->getHeight()) return;

    area.start.x = std::max(area.start.x, 0);
    area.start.y = std::max(area.start.y, 0);
    area.end.x = std::min(area.end.x, _sketch->getWidth());
    area.end.y = std::min(area.end.y, _sketch->getHeight());

    size_t activeFrameIndex = getFrameIndex(activeFrame->getID());
    Layer* activeLayer = activeFrame->getActiveLayer();
    size_t activeLayerIndex = activeFrame->getLayerIndex(activeLayer->getID());

    float opacity = 0.3;
    for(int y = area.start.y; y < area.end.y; y++){
        int index = y * _sketch->getWidth() + area.start.x;
        
        for(int x = area.start.x; x < area.end.x; x++){
            unsigned int colorHex = activeFrameIndex > 0 ? frames[activeFrameIndex - 1].get()->getPixel(index) : 0;
            GraphicsEngine::setOpacity(colorHex, opacity);

            colorHex = GraphicsEngine::blendColors(colorHex, activeFrame->getPixel(index, 0, activeLayerIndex));
            colorHex = GraphicsEngine::blendColors(colorHex, _preview->getPixel(index));
            colorHex = GraphicsEngine::blendColors(colorHex, activeFrame->getPixel(index, activeLayerIndex+1, activeFrame->getLayersLength()));

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
    
    compose();

    return removedFrame;
}
void Editor::changeActiveFrame(Guid id){
    activeFrame = getFrameByID(id);
    
    _preview = new Preview(_sketch->getWidth(), _sketch->getHeight());
    _preview->setTarget(activeFrame->getActiveLayer());
    compose();

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

    compose();
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

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    enum_<EDITOR_EVENT_TYPE>("EDITOR_EVENT_TYPE")
        .value("DRAW", EDITOR_EVENT_TYPE::DRAW)
        .value("ADD_FRAME", EDITOR_EVENT_TYPE::ADD_FRAME)
        .value("REMOVE_FRAME", EDITOR_EVENT_TYPE::REMOVE_FRAME)
        .value("MOVE_FRAME_TO", EDITOR_EVENT_TYPE::MOVE_FRAME_TO)
        .value("CHANGE_ACTIVE_FRAME", EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME);
};