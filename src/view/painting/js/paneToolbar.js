import HandlerEvents from './handlerEvents.js'

const handlerEvents = HandlerEvents(document.querySelector("canvas#painting"));

export function buildPaneToolBar(){
    const btnPencil = document.querySelector(".tool-pencil");
    btnPencil.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(paintStrategy());
        changeSelectTool.call(this);
    });
    const btnEraser = document.querySelector(".tool-eraser");
    btnEraser.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(eraseStrategy());
        changeSelectTool.call(this);
    });
    const btnDropper = document.querySelector(".tool-dropper");
    btnDropper.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(dropperStrategy());
        changeSelectTool.call(this);
    });

    const btnLine = document.querySelector(".tool-line");
    btnLine.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(lineStrategy());
        changeSelectTool.call(this);
    });
    const btnSquare = document.querySelector(".tool-square");
    btnSquare.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(squareStrategy());
        changeSelectTool.call(this);
    });

    const btnCircle = document.querySelector(".tool-circle");
    btnCircle.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(circleStrategy());
        changeSelectTool.call(this);
    });

    const btnBucket = document.querySelector(".tool-bucket");
    btnBucket.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(bucketStrategy());
        changeSelectTool.call(this);
    });

    const btnBrush = document.querySelector(".tool-brush");
    btnBrush.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(brushStrategy());
        changeSelectTool.call(this);
    });
    const btnSelect = document.querySelector(".tool-select");
    btnSelect.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(selectStrategy());
        changeSelectTool.call(this);
    });

    const btnUndo = document.querySelector("#undo");
    btnUndo.addEventListener("click", function(e){
        history.undo();
    });

    const btnRedo = document.querySelector("#redo");
    btnRedo.addEventListener("click", function(e){
        history.redo();
    });

    handlerEvents.setScrollEvent(onZoomScrollStrategy(), false);
    handlerEvents.setDoubleTouchEvent(onZoomDoubleTouchStrategy(), false);
    handlerEvents.setDoubleTouchEvent(onZoomDoubleTouchStrategy(), false);
    handlerEvents.setScrollEvent(onSizeStrategy(), true);
    handlerEvents.setGenericButtonMousePressedEvent(onPanningStrategy());
    handlerEvents.setLeftButtonMousePressedEvent(eraseStrategy());
    // handlerEvents.setRightButtonMousePressedEvent(onPanningStrategy());

    btnPencil.click();
}