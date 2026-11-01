import ModulePixelEditor from '../build/PixelEditor.js'
import { Shortcuts } from './shortcuts.js';
import {repository} from "./repository.js"

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

    const _shortcuts = Shortcuts();
    const _database = await repository();

    app = Object.freeze({
        canvas,
        resize: module.resize,
        database: _database,
        malloc: (buffer)=>{
            const ptr =  module._malloc(buffer.byteLength);
            module.HEAPU8.set(buffer, ptr);
            return ptr;
        },
        
        editorManagerViewModel: ()=> { return new module.EditorManagerViewModel() },
        editorVM: (editorId)=> { return new module.EditorVM(editorId); },
        frameVM: (editorId, frameId)=> { return new module.FrameVM(editorId, frameId); },
        layerViewModel: (editorId, frameId, layerID)=> { return new module.LayerViewModel(editorId, frameId, layerID); },
        paneToolViewModel: new module.PaneToolbarViewModel(),
        drawingSettingsVM:  new module.DrawingSettingsVM(),
        brushSettingsVM: new module.BrushSettingsVM(),
        symmetrySettingsVM:  new module.SymmetrySettingsVM(),
        shortcuts: _shortcuts
    });


}


export { init, app }