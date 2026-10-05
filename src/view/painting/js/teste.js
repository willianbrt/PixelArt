import { init, app } from './app.js'
import { buildPaneFrames } from "./paneFrame.js"
import { buildPaneLayers } from "./paneLayer.js"
import { buildPaneToolBar } from './paneToolbar.js';
import { buildPanePalette } from './panePalette.js';
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
    
    app.editorManagerViewModel.registerEvent("ADD_EDITOR", buildEditor);
    app.editorManagerViewModel.createProject(32, 32); 

    channel.postMessage({ action: "REQUEST_CLIPBOARD"});

    buildPanePalette();
    buildShortcuts();
}

function buildEditor(){ 
    console.log("j")
    buildPaneFrames(app.editorVM());
    buildPaneLayers(app.frameVM());
    buildPaneToolBar();

    
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
            const surface = app.editorManagerViewModel.copy();
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
            app.editorManagerViewModel.paste()
        }
    });
}
function buildShortcuts(){ 
}