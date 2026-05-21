import { init, app } from './app.js'
import { buildPaneFrames } from "./paneFrame.js"
import { buildPaneLayers } from "./paneLayer.js"
import { buildPaneToolBar } from './paneToolbar.js';
import { Shortcuts } from './shortcuts.js';

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
    
    buildPaneFrames(app.paneFramesViewModel());
    buildPaneLayers(app.paneLayersViewModel());
    buildPaneToolBar();
    buildShortcuts();

    channel.postMessage({ action: "REQUEST_CLIPBOARD"});
}

function buildShortcuts(){    
    app.shortcuts.register({
        default:{
            ctrl: true,
            shitft: false,
            alt: false,
            keyCode: 67,
        },
        description: "teste",
        scope: "global",
        callback: ()=>{console.log("copiar")}
    });
}