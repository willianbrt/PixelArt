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
        console.log("set",clipboard)
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
    app.shortcuts.register({
        default:{
            ctrl: true,
            shitft: false,
            alt: false,
            keyCode: 67,
        },
        description: "teste",
        scope: "global",
        callback: ()=>{
            console.log("copiar")
            const surface = editorManagerViewModel.copy();
            clipboard = surface;
            channel.postMessage({ action: "SET_CLIPBOARD", clipboard: surface});
        }
    });
    app.shortcuts.register({
        default:{
            ctrl: true,
            shitft: false,
            alt: false,
            keyCode: 86,
        },
        description: "teste",
        scope: "global",
        callback: ()=>{
            console.log("colar",clipboard)
            const { buffer, width } = clipboard;
            const height = buffer.length / 4 / width;
            editorManagerViewModel.paste(clipboard)
        }
    });
}

function buildShortcuts(){ 
}