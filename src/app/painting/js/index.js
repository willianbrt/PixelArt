import {preventFunction} from "./../../../scripts/common/preventFunction.js"
import { Draft } from "./draft.js" 

(()=>{
    const canvas = document.querySelector("#paintingCanvas");
    const draft = Draft({
        canvas: canvas, 
        sketchWidth: 100,
        sketchHeight: 100
    });
    draft.draw();
    let scale = draft.getScale();

    canvas.addEventListener("mousedown", onMouseKeyDown);
    canvas.addEventListener("mouseup", onMouseKeyUp);
    canvas.addEventListener("mousemove", onMouseMove);
    canvas.addEventListener("wheel", preventFunction(onScroll));
    
    
    function onMouseMove(event){
        // TODO: IMPLEMENTAR FUNÇÃO
        let cursorPosition = getPositionCursor(event);
        draft.hover(cursorPosition.x, cursorPosition.y)
    }
        
        
    function onScroll(event){
        event.preventDefault();

        let positionCursor = getPositionCursor(event);
        
        if(event.deltaY > 0)
            draft.zoomIn(positionCursor.x, positionCursor.y);
        else
            draft.zoomOut(positionCursor.x, positionCursor.y);
    }

    function getPositionCursor(event){
        let canvasPosition = canvas.getBoundingClientRect();
        
        return Object.freeze({
            x: Math.floor(event.clientX - canvasPosition.left),
            y: Math.floor(event.clientY - canvasPosition.top) 
        });
    }

    let onMoveController;
    function onMouseKeyDown(event){
        event.preventDefault();
        onMoveController = new AbortController();
        
        let startPositionCursor = getPositionCursor(event);
        let flagPositionCursorX = startPositionCursor.x;
        let flagPositionCursorY = startPositionCursor.y;

        const acceptActions = {
            2: ()=>{ 
                canvas.style.cursor = "grabbing";
                canvas.addEventListener("mousemove", onMove.bind(null, flagPositionCursorX, flagPositionCursorY), { signal: onMoveController.signal });
                canvas.addEventListener("mouseout", stopMove, {once: true, signal: onMoveController.signal});
            },
        }

        if(isActionValid(acceptActions, event.which)) throw new Error("Ação inválida!");

        acceptActions[event.which]();
    }

    function onMove(flagPositionCursorX, flagPositionCursorY, event){
        event.preventDefault();

        let positionCursor = getPositionCursor(event);
        draft.panning(flagPositionCursorX, flagPositionCursorY, positionCursor.x, positionCursor.y);

        flagPositionCursorX = positionCursor.x;
        flagPositionCursorY = positionCursor.y;
    }

    function onMouseKeyUp(event){
        event.preventDefault();
        
        const acceptActions = {
            2: stopMove,
        }
        
        if(isActionValid(acceptActions, event.which)) throw new Error("Ação inválida!");
        
        acceptActions[event.which]();
    }
    function stopMove(){
        canvas.style.cursor = "";
        onMoveController.abort();
    }

    function isActionValid(acceptActions, action) { return acceptActions[action] === undefined } 
})();
