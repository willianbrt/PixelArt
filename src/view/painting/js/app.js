import ModulePixelEditor from '../build/PixelEditor.js'
import { Shortcuts } from './shortcuts.js';

var app;

async function init(){
    const canvas = document.querySelector("#painting");

    if(!canvas) {
        throw new Error("Canvas não encontrado");
    }
    const module = await ModulePixelEditor({
        canvas:  canvas,
        preRun: function() {},
        postRun: ()=>{},
        onRuntimeInitialized: () =>{}
    });

    const shortcuts = Shortcuts();

    app = Object.freeze({
        canvas,
        resize: module.resize,
        editorManagerViewModel: ()=> { return new module.EditorManagerViewModel(); },
        paneFramesViewModel: (editor)=> { return new module.PaneFramesViewModel(); },
        paneLayersViewModel: (frame)=> { return new module.PaneLayersViewModel(); },
        paneToolViewModel: (frame)=> { return new module.PaneToolbarViewModel(); },
        layerViewModel: (layerID)=> { return new module.LayerViewModel(layerID); },
        shortcuts
    });


}


export { init, app }