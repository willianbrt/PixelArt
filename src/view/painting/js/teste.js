import ModulePixelEditor from '../build/PixelEditor.js'
import { buildPaneFrames } from "./paneFrame.js"
import { buildPaneLayers } from "./paneLayer.js"

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
        canvas:  document.querySelector("canvas#painting"),
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
    try{
        const editorViewModel = module.createProject(32, 32);

        buildPaneFrames(editorViewModel);        
        buildPaneLayers(editorViewModel.getLayersViewModel());
    }catch(e){
        console.log(e)
    }
}