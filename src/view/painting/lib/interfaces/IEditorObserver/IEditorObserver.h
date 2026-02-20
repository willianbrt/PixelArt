#ifndef IEDITOROBSERVER_H
#define IEDITOROBSERVER_H

#include "../../objects/frame/Frame.h"

class IEditorObserver{
public:
    virtual void onChangeActiveFrame(Guid id) = 0;
    virtual void onAddFrame(Frame* frame, size_t index) = 0;
    virtual void onRemoveFrame(Guid id) = 0;
    virtual void onMoveFrameTo(Guid id, int index) = 0;
};

#endif