#include "AppContext.h"

AppContext::AppContext(){

}
void AppContext::build(){
    _editorManager = std::make_unique<EditorManager>();
}
EditorManager* AppContext::getEditorManager(){
    printf("EditorManager: %p\n", _editorManager.get());
    return _editorManager.get();
}
AppContext& AppContext::instance(){
    static AppContext appContext;
    return appContext;
}