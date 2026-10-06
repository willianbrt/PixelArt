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
let _editorManagerViewModel;
window.onload = async ()=>{
    await init();
    
    _editorManagerViewModel = await app.editorManagerViewModel();
    _editorManagerViewModel.registerEvent("ADD_EDITOR", buildEditor);
    _editorManagerViewModel.createProject(32, 32); 

    channel.postMessage({ action: "REQUEST_CLIPBOARD"});

    buildPanePalette();
    buildShortcuts();
}

async function buildEditor(){ 
    const editorVM = await app.editorVM();

    const paneFrame = await buildPaneFrames(editorVM);
    _editorManagerViewModel.registerEvent("CHANGE_ACTIVE_EDITOR", paneFrame.onChangeEditor);

    const paneLayer = await buildPaneLayers(await app.frameVM());
    editorVM.registerEvent("CHANGE_ACTIVE_FRAME", paneLayer.onChangeFrame);

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
            const surface = _editorManagerViewModel.copy();
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
            _editorManagerViewModel.paste()
        }
    });
}
function buildShortcuts(){ 
}