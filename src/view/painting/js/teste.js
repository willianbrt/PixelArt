import { init, app } from './app.js'
import { buildPaneFrames } from "./paneFrame.js"
import { buildPaneLayers } from "./paneLayer.js"
import { buildPaneToolBar } from './paneToolbar.js';

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
    const drawingArea = document.querySelector("#drawing-area");

    await init(250, 500);
    // await init(drawingArea.clientWidth, drawingArea.clientHeight);
    
    const editorManagerViewModel = app.editorManagerViewModel();
    
    editorManagerViewModel.createProject(32, 32); 
    
    buildPaneFrames(app.paneFramesViewModel());
    buildPaneLayers(app.paneLayersViewModel());
    buildPaneToolBar();


    window.addEventListener("resize", (e)=>{
        app.resize(drawingArea.clientWidth,drawingArea.clientHeight);
    });

    channel.postMessage({ action: "REQUEST_CLIPBOARD"});
}