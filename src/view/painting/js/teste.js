import { init, app } from './app.js'
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
    await init();
    const editorManagerViewModel = app.editorManagerViewModel();
    
    editorManagerViewModel.createProject(32, 32); 
    console.clear()
    buildPaneFrames(app.paneFramesViewModel());
    buildPaneLayers(app.paneLayersViewModel());


    channel.postMessage({ action: "REQUEST_CLIPBOARD"});
}