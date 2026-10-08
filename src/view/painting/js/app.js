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
        
        editorManagerViewModel: ()=> { return new module.EditorManagerViewModel() },
        editorVM: (editorId)=> { return new module.EditorVM(editorId); },
        frameVM: (editorId, frameId)=> { return new module.FrameVM(editorId, frameId); },
        paneToolViewModel: (frame)=> { return new module.PaneToolbarViewModel(); },
        layerViewModel: (editorId, frameId, layerID)=> { return new module.LayerViewModel(editorId, frameId, layerID); },
        drawingSettingsVM: (layerID)=> { return new module.DrawingSettingsVM(); },
        brushSettingsVM: (layerID)=> { return new module.BrushSettingsVM(); },
        symmetrySettingsVM: (layerID)=> { return new module.SymmetrySettingsVM(); },
        shortcuts: _shortcuts
    });


}


export { init, app }