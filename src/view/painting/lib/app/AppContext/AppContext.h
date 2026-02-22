#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "../../objects/EditorManager/EditorManager.h"

class AppContext{
private:
    std::unique_ptr<EditorManager> _editorManager;
public:
    AppContext();
    
    void build();
    EditorManager* getEditorManager();

    static AppContext& instance();
};
#endif
