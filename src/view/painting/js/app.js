import ModulePixelEditor from '../build/PixelEditor.js'

var app;

async function init(){
    const module = await ModulePixelEditor({
        canvas:  document.querySelector("canvas#painting"),
        preRun: function() {
            console.log("WASM module is starting...");
        },
        postRun: ()=>{
            console.log("Done");
        },
        onRuntimeInitialized: () =>{
            console.log("MODULE INITIALIZED")
        }
    });

    module.initApp();

    app = Object.freeze({
        editorManagerViewModel: ()=> { return new module.EditorManagerViewModel(); },
        paneFramesViewModel: (editor)=> { return new module.PaneFramesViewModel(); },
        paneLayersViewModel: (frame)=> { return new module.PaneLayersViewModel(); },
        layerViewModel: (layerID)=> { return new module.LayerViewModel(layerID); }
    });
}


export { init, app }