#include "DrawFrameCommand.h"

DrawFrameCommand::DrawFrameCommand(Editor& editor, Guid frameId, IGraphic& graphic) : _editor(editor),_graphic(graphic){}
DrawFrameCommand::~DrawFrameCommand(){}
void DrawFrameCommand::execute(){}
void DrawFrameCommand::undo(){}