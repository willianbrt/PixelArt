#ifndef IEDITORMANAGEROBSERVER_H
#define IEDITORMANAGEROBSERVER_H

#include "../../objects/Editor/Editor.h"

class IEditorManagerObserver{
public:
    virtual void onChangeActiveEditor(Guid id) = 0;
    virtual void onAddEditor(Editor* editor, size_t index) = 0;
    virtual void onRemoveEditor(Guid id) = 0;
    virtual void onMoveEditorTo(Guid id, int index) = 0;
};

#endif