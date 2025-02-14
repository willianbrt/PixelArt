import ModulePixelEditor from '../build/graphics/PixelEditor.js'
import HandlerEvents from './handlerEvents.js'
import History from './invokers/history.js'

const canvas = document.querySelector("canvas");
const handlerEvents = HandlerEvents(canvas);
const history = History();

let width = 100;
let height = 100;

window.onload = async ()=>{
    window.module = await ModulePixelEditor({ canvas: canvas,
        preRun: function() {
            console.log("WASM module is starting...");
        },
        postRun: function() {
            console.log("WASM module has completed...");
        },
        onRuntimeInitialized: () =>{
            console.log("MODULE INITIALIZED")
        }
    });

    window.editor = new module.PixelEditor(width, height);
    editor.render();
    editor.hover(0,0);

    // let frame = new module.Frame(width, height);
    // editor.addTile(frame);
    // console.log(frame)
    
    // const activeFrame = editor.getActiveTile();
    // console.log(activeFrame)


    // for(let i  = 0; i < 10; i++){
    //     let name = "Frame " + (editor.getNumberOfTiles()+1);
    //     let frame = new module.Frame(width, height);
    //     editor.addTile(frame);
    //     console.log(name)
    // }

    
    // let layers = activeFrame.getAllTiles();
    // for(let i  = 0; i < 10; i++){
    //     let name = "Layer " + (layers.size()+1);
        
    //     let layer = new module.Layer(name, width, height);
    //     activeFrame.addTile(layer);
    // }
    // const activelayer = activeFrame.getActiveTile();
    
    // console.log(frames)
    // console.log(frames.get(0))
    // console.log(activelayer)
    // console.log(activelayer.getName())
    // addFrames(frames)
    // addLayer(layers);

    buildToolBar();

    const standartEvents = getStandardEvents();
    standartEvents.restoreScroll();
    standartEvents.restoreLeftPressed();
    // standartEvents.restoreRightPressed();
    // standartEvents.restoreMiddlePressed();
};

function buildLayer(layer){
    let isVisible = layer.isVisible();
    let name = layer.getName();
}

function addLayer(){
    let frame = editor.getActiveTile();

    let length = frame.getNumberOfTiles();
   console.log(length) 
    for(let i = 0; i < length; i++){
        let layer = frame.getTileByIndex(i);

        console.group(layer.getName());
        console.log("visivel: ", layer.isVisible());
        console.log(layer.getName());
        console.groupEnd(layer.getName());
    }
}

function addFrames(){
    let length = editor.getNumberOfTiles();
    console.log(length) 
    for(let i = 0; i < length; i++){
        let frame = editor.getTileByIndex(i);

        console.group("Frame " + i);
        console.log("duration: ", frame.getFrameDuration());
        console.log("ativo: ", i == editor.getActiveTile());
        console.groupEnd("Frame " + i);
    }
}

function buildToolBar(){    
    const buttonPencil = document.querySelector(".tool-pencil");
    buttonPencil.addEventListener("click", (e)=>{
        handlerEvents.setRightButtonMousePressedEvent(new module.PencilPressed(editor));
    });
    
    const buttonEraser = document.querySelector(".tool-eraser");
    buttonEraser.addEventListener("click", (e)=>{
        // handlerEvents.setRightMousePressedEvent(EraserTool);
    });
    
    const buttonDropper = document.querySelector(".tool-dropper");
    buttonDropper.addEventListener("click", (e)=>{
        // handlerEvents.setRightMousePressedEvent(DropperTool);
    });
    
    const buttonLine = document.querySelector(".tool-line");
    buttonLine.addEventListener("click", (e)=>{
        // handlerEvents.setRightMousePressedEvent(LineTool);
    });
    
    const buttonUndo = document.querySelector(".tool-undo");
    buttonUndo.addEventListener("click", (e)=>{
        history.undo();
    });
    
    const buttonRedo = document.querySelector(".tool-redo");
    buttonRedo.addEventListener("click", (e)=>{
        history.redo();
    });
    
    const buttonAddframe = document.querySelector(".tool-addframe");
    buttonAddframe.addEventListener("click", (e)=>{
        console.clear()

        let frame = new module.Frame(width, height);
        editor.addTile(frame);
        
        addFrames(editor.getAllTiles());
    });
    
    const buttoAddlayer = document.querySelector(".tool-addlayer");
    buttoAddlayer.addEventListener("click", (e)=>{
        console.clear();
        const activeTile = editor.getActiveTile();
        
        let name = "Layer " + (activeTile.getNumberOfTiles()+1);
        
        let layer = new module.Layer(name, width, height);
        activeTile.addTile(layer);

        addLayer();
    });
}

function getStandardEvents(){
    // handlerEvents.setGenericButtonMousePressedEvent(new module.PanningStrategy(editor));
    // handlerEvents.setScrollEvent(new module.ZoomScroll(editor));
    return Object.freeze({
        restoreScroll:          ()=>{ module.defineScrollEvent(new module.ZoomScroll(editor)); },
        restoreLeftPressed:     ()=>{ module.defineLeftPressedEvent(new module.PanningPressed(editor)); },
        restoreRightPressed:    ()=>{ module.defineScrollEvent(new module.ZoomScroll(editor)); },
        restoreMiddlePressed:   ()=>{ module.defineScrollEvent(new module.ZoomScroll(editor)); },
    });
}

window.addEventListener("contextmenu", (event)=>{
    event.preventDefault();
});