import ModulePixelEditor from '../build/PixelEditor.js'
import { buildPaneFrames } from "./paneFrame.js"

const canvas = document.querySelector("canvas#painting");

let clipboard;
const channel = new BroadcastChannel("shared-buffer");
channel.onmessage = (e) => {
    if(e.data.action == "SET_CLIPBOARD"){
        clipboard = e.data.clipboard;
    }

    if(e.data.action == "REQUEST_CLIPBOARD"){
        channel.postMessage({ action:"SET_CLIPBOARD", clipboard: clipboard});
    }
};
window.onload = async ()=>{
    const module = await ModulePixelEditor({
        canvas,
        preRun: function() {
            console.log("WASM module is starting...");
        },
        postRun: moduleInitalized,
        onRuntimeInitialized: () =>{
            console.log("MODULE INITIALIZED")
            channel.postMessage({ action: "REQUEST_CLIPBOARD"});
        }
    });
    
}
function moduleInitalized(module) {
    console.log("WASM module has completed...");
    try{
        const editor = createProject.apply(module, [32, 32]);
        const editorViewModel = new module.EditorViewModel(editor);
        buildPaneFrames.apply(module, [editorViewModel]);
    }catch(e){
        console.log(e)
    }
}
function createProject(width, height){
    const editor = new this.Editor(width, height);
    let frame = new this.Frame();
    let layer = new this.Layer("DEFAULT_NAME_LAYER", width, height);
    frame.addLayer(layer);
    editor.addFrame(frame,0);

    return editor;
}