import {preventFunction} from "./../../../scripts/common/preventFunction.js"
import { Draft } from "./draft.js" 

(()=>{
    const canvas = document.querySelector("#paintingCanvas");
    const draft = Draft({
        canvas: canvas, 
        sketchWidth: 50,
        sketchHeight: 100
    });
    draft.draw();

    canvas.addEventListener("mousedown", onMouseKeyDown);
    canvas.addEventListener("mouseup", onMouseKeyUp);
    canvas.addEventListener("mousemove", onMouseMove);
    canvas.addEventListener("wheel", preventFunction(onScroll));
    
    
    function onMouseMove(event){
        // TODO: IMPLEMENTAR FUNÇÃO
        let cursorPosition = getPositionCursor(event);
        draft.onHover(cursorPosition.x, cursorPosition.y)
    }
        
        
    function onScroll(event){
        event.preventDefault();

        let positionCursor = getPositionCursor(event);
        
        if(event.deltaY < 0)
            draft.zoomIn(positionCursor.x, positionCursor.y);
        else
            draft.zoomOut();
    }

    function getPositionCursor(event){
        let canvasPosition = canvas.getBoundingClientRect();
        
        return Object.freeze({
            x: Math.floor(event.clientX - canvasPosition.left),
            y: Math.floor(event.clientY - canvasPosition.top) 
        });
    }

    let onMoveSignal = new AbortController();
    function onMouseKeyDown(event){
        event.preventDefault();
        
        let startPositionCursor = getPositionCursor(event);

        const acceptActions = {
            2: panning(startPositionCursor),
        }[event.which]?.();
    }

    function panning(startPositionCursor){
        let flagPositionCursorX = startPositionCursor.x;
        let flagPositionCursorY = startPositionCursor.y;
        
        canvas.style.cursor = "grabbing";
        canvas.addEventListener("mousemove", (e)=>{
            e.preventDefault();
            
            let positionCursor = getPositionCursor(e);
            draft.panning(flagPositionCursorX, flagPositionCursorY, positionCursor.x, positionCursor.y);
            
            flagPositionCursorX = positionCursor.x;
            flagPositionCursorY = positionCursor.y;
        }, { signal: onMoveSignal.signal });
        
        canvas.addEventListener("mouseout", stopMove, {once: true, signal: onMoveSignal.signal});
    }

    function onMouseKeyUp(event){
        event.preventDefault();
        
        const acceptActions = {
            2: stopMove,
        }[event.which]?.();
    }
    function stopMove(){
        canvas.style.cursor = "";

        onMoveSignal.abort();
        onMoveSignal = new AbortController();
    }
})();
