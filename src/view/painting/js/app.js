import ModulePixelEditor from '../build/PixelEditor.js'

var app;

async function init(width, height){
    const canvas = document.querySelector("#painting");

    if(!canvas) {
        throw new Error("Canvas não encontrado");
    }
    const module = await ModulePixelEditor({
        canvas:  canvas,
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

    module.initApp(width, height);

    app = Object.freeze({
        canvas,
        resize: module.resize,
        editorManagerViewModel: ()=> { return new module.EditorManagerViewModel(); },
        paneFramesViewModel: (editor)=> { return new module.PaneFramesViewModel(); },
        paneLayersViewModel: (frame)=> { return new module.PaneLayersViewModel(); },
        layerViewModel: (layerID)=> { return new module.LayerViewModel(layerID); },
        brushStrategy: (brushContext,drawingContext)=> { return new module.BrushStrategy(brushContext, drawingContext); },
        brushContext: ()=> { return new module.BrushContext(); },
        drawingContext: ()=> { return new module.DrawingContext(); },
    });
}


export { init, app }