import HandlerEvents from './handlerEvents.js'
import History from './invokers/history.js'
import Layer from "./recivers/layer.js" 
import Sketch from './recivers/sketch.js'
import PanningTool from './strategies/panningPressed.js'
import ZoomTool from './strategies/zoomWheel.js'
import PencilTool from './strategies/pencilPressed.js'
import PencilPressed from './strategies/pencilPressed.js'
// import EraserTool from './strategies/eraserPressed.js'
// import LineTool from './strategies/line.js'
    

const canvas = document.querySelector("canvas");
const history = History();
const handlerEvents = HandlerEvents(canvas);
let sketch;

(async()=>{
    await initSketch();
    setEventsDefault();
    buildToolBar();
})();

async function initSketch(){    
    sketch = await Sketch({
        canvas: canvas,
        width: 100,
        height: 100,
    });
}


function buildToolBar(){    
    const buttonPencil = document.querySelector(".tool-pencil");
    buttonPencil.addEventListener("click", (e)=>{
        handlerEvents.setRightButtonMousePressedEvent(new PencilPressed(sketch));
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
}
function setEventsDefault(){
    handlerEvents.setGenericButtonMousePressedEvent(new PanningTool(sketch));
    handlerEvents.setScrollEvent(new ZoomTool(sketch));
}

window.addEventListener("contextmenu", (event)=>{
    event.preventDefault();
});