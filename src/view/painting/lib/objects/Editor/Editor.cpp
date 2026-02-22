#include "Editor.h"

Editor::Editor(int width, int height) : _sketch(width, height) {
}
Editor::~Editor(){
    frames.clear();
    activeFrame = nullptr;
}

void Editor::registerEvent(IEditorObserver* observer){
    observers.push_back(observer);
}
Bounding Editor::getSketchBounding(){
    Point endPoint = Point(_sketchPosition.x + _sketch.getWidth(), _sketchPosition.y + _sketch.getHeight());
    return Bounding(_sketchPosition, endPoint);
}

void Editor::setNumberTiles(int rol, int col){
    _rows = rol; 
    _cols = col; 
}

void Editor::preview(IGraphic& graphic){
    activeFrame->preview(graphic);
    render();
}

void Editor::draw(IGraphic& graphic){
    activeFrame->draw(graphic);
    render();
}

void Editor::render(){
    Point endPoint = Point(_sketch.getWidth(), _sketch.getHeight());
    Bounding boundingSketch = Bounding(Point(0,0), endPoint);

    renderArea(boundingSketch);
}
void Editor::render(int startX, int endX, int startY,int endY){
    Bounding boundingSketch = Bounding(Point(startX,startY), Point(endX, endY));

    renderArea(boundingSketch);
}
void Editor::renderArea(Bounding area){
    if(area.start.x > _sketch.getWidth() || area.start.y > _sketch.getHeight()) return;

    area.start.x = (area.start.x < 0) ? 0 : area.start.x;
    area.start.y = (area.start.y < 0) ? 0 : area.start.y;
    area.end.x = (area.end.x > _sketch.getWidth()) ? _sketch.getWidth() : area.end.x;
    area.end.y = (area.end.y > _sketch.getHeight()) ? _sketch.getHeight() : area.end.y;

    size_t activeIndex = getFrameIndex(activeFrame->getID());
    Frame* previousFrame = frames[activeIndex > 0 ? activeIndex - 1 : 0].get();
    float opacity = 0.5;
    for(int y = area.start.y; y < area.end.y; y++){
        int index = y * _sketch.getWidth();
        for(int x = area.start.x; x < area.end.x; x++){
            unsigned int previousColorHex = previousFrame->getPixel(index);
            previousColorHex = (previousColorHex & 0xFFFFFF00) | static_cast<int>(opacity * (previousColorHex & 0xFF));

            unsigned int colorHex = GraphicsEngine::blendColors(previousColorHex, activeFrame->getPixel(index));
            // swap_endian_uint32(&colorHex);
            _sketch.putPixel(index, colorHex);

            index++;
        }
    }

    renderCanvas(
        _sketch.getWidth(), _sketch.getHeight(),
        _sketch.getBuffer(), _sketch.getLength(), 
        _sketch.getWidth(), _sketch.getHeight(), 
        area.start.x, area.start.y,
        _rows, _cols);
}

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
    return removedFrame;
}
void Editor::changeActiveFrame(Guid id){
    activeFrame = getFrameByID(id);
    
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

int Editor::getWidth(){
    return _sketch.getWidth();
}
int Editor::getHeight(){
    return _sketch.getHeight();
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