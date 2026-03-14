import { PositionHelper } from "../../../scripts/common/position.js";

export default function HandlerEvents(canvas){
    const KEY_MOUSE = {
        RIGHT_BUTTON:0,
        WHELL_BUTTON:1,
        LEFT_BUTTON:2,
    };
    const noEvent = {
        down: ()=>{},
        move: ()=>{},
        up:()=>{},
        dispatch: ()=>{}
    };
    
    let buttonMousePressed = undefined;
    let touchID = undefined;
    let touchTimer = null;

    let leftButtonEvent = noEvent;
    let rightButtonEvent = noEvent;
    let otherButtonPressedEvent = noEvent;
    let scrollEvents = {
        default: { onScrollUp: ()=>{}, onScrollDown: ()=>{} },
        ctrl: { onScrollUp: ()=>{}, onScrollDown: ()=>{} }
    };
    let noTouchEvents = {
        down: (firstTouche, secondTouche)=>{},
        move: (firstTouche, secondTouche)=>{},
        up: (firstTouche, secondTouche)=>{}
    };
    let singleTouchEvent = noTouchEvents;
    let doubleTouchEvent = noTouchEvents;
    let moveEvent = ()=>{};
    let preventDefaultMoveEvent = false;
    
    function setScrollEvent(eventHandler, enableCtrlKey = false){
        if (enableCtrlKey == true)
            scrollEvents.ctrl = eventHandler;
        else
            scrollEvents.default = eventHandler;
    }
    function setMoveEvent(eventHandler){
        moveEvent = eventHandler; 
    }
    function setClickLeftEvent(handlerEvent){
        leftButtonEvent.down = noEvent;
        leftButtonEvent.up = handlerEvent.click;
    }  
    function setClickRightEvent(handlerEvent){
        rightButtonEvent.down = noEvent;
        rightButtonEvent.up = handlerEvent.click;
    }
    function setGenericButtonMousePressedEvent(strategy){
        otherButtonPressedEvent.dispatch?.();
        otherButtonPressedEvent = createMousePressedEvent(strategy);
    }
    function setRightButtonMousePressedEvent(strategy){
        rightButtonEvent.dispatch?.();
        rightButtonEvent = createMousePressedEvent(strategy);
        setSingleTouchEvent(strategy);
    }
    function setLeftButtonMousePressedEvent(strategy){
        leftButtonEvent.dispatch?.();
        leftButtonEvent = createMousePressedEvent(strategy);
    }

    function createMousePressedEvent(strategy) {
        return {
            down:(event)=>{
                const point = PositionHelper.getPositionCursor(event.clientX, event.clientY, canvas);
                strategy.onPressed(point.x, point.y);
            },
            move:(event)=>{
                const point = PositionHelper.getPositionCursor(event.clientX, event.clientY, canvas);
                preventDefaultMoveEvent = true;
                
                requestAnimationFrame(()=>{
                        try{
                        strategy.onTracking(point.x, point.y)
                    }catch(e){
                    console.log(e)
                }
                    });
                
            },
            up:(event)=>{
                const point = PositionHelper.getPositionCursor(event.clientX, event.clientY, canvas);
                preventDefaultMoveEvent = false;
                requestAnimationFrame(()=>strategy.onRelease(point.x, point.y));
            },
            dispatch: strategy.dispatch
        };
    }
    function setSingleTouchEvent(strategy){
        singleTouchEvent.dispatch?.();
        singleTouchEvent = {
            down:(event)=>{
                strategy.onPressed(PositionHelper.getPositionCursor(event.touches[0].clientX, event.touches[0].clientY, canvas),event);
            },
            move:(event)=>{
                preventDefaultMoveEvent = true;
                requestAnimationFrame(()=>strategy.onTracking(PositionHelper.getPositionCursor(event.touches[0].clientX, event.touches[0].clientY, canvas)));
            },
            up:(event)=>{
                preventDefaultMoveEvent = false;
                requestAnimationFrame(()=>strategy.onRelease(PositionHelper.getPositionCursor(event.changedTouches[0].clientX, event.changedTouches[0].clientY, canvas)));
            },
            dispatch: strategy.dispatch
        };
    }
    function setDoubleTouchEvent(strategy){
        doubleTouchEvent.dispatch?.();
        doubleTouchEvent = {
            down:(event)=>{
                strategy.onPressed(
                    PositionHelper.getPositionCursor(event.touches[0].clientX, event.touches[0].clientY, canvas),
                    PositionHelper.getPositionCursor(event.touches[1].clientX, event.touches[1].clientY, canvas)
                );
            },
            move:(event)=>{
                requestAnimationFrame(()=>strategy.onTracking(
                    PositionHelper.getPositionCursor(event.touches[0].clientX, event.touches[0].clientY, canvas),
                    PositionHelper.getPositionCursor(event.touches[1].clientX, event.touches[1].clientY, canvas)
                ));
            },
            up:( event)=>{
                requestAnimationFrame(()=>strategy.onRelease(
                    PositionHelper.getPositionCursor(event.touches[0].clientX, event.touches[0].clientY, canvas),
                    PositionHelper.getPositionCursor(event.changedTouches[0].clientX, event.changedTouches[0].clientY, canvas)
                ));
            },
            dispatch: strategy.dispatch
        };
    }
    

    canvas.addEventListener("mouseleave", (event)=>{
        event.preventDefault();
    });
    canvas.addEventListener("wheel", (event)=>{
        event.preventDefault();

        let eventHandler = scrollEvents.default;

        if (event.ctrlKey){
            eventHandler = scrollEvents.ctrl
        }

        requestAnimationFrame(()=>{
            if(event.deltaY < 0)
                eventHandler.onScrollUp(PositionHelper.getPositionCursor(event.clientX, event.clientY, canvas))
            else
                eventHandler.onScrollDown(PositionHelper.getPositionCursor(event.clientX, event.clientY, canvas))
        });
    });
    canvas.addEventListener("mousedown", (event)=>{
        event.preventDefault();
        
        if(!(buttonMousePressed === undefined)) return;
        
        switch(event.button){
            case KEY_MOUSE.RIGHT_BUTTON: rightButtonEvent.down(event); break;
            case KEY_MOUSE.LEFT_BUTTON: leftButtonEvent.down(event); break;
            default: otherButtonPressedEvent.down(event); break;
        }

        buttonMousePressed = event.button;
    });
    canvas.addEventListener("mousemove", function (event){
        event.preventDefault();
        // if(!preventDefaultMoveEvent)
        //     moveEvent(PositionHelper.getPositionCursor(event.clientX, event.clientY, canvas));

        if(buttonMousePressed === undefined) return;

        switch(buttonMousePressed){
            case KEY_MOUSE.RIGHT_BUTTON: rightButtonEvent.move(event); break;
            case KEY_MOUSE.LEFT_BUTTON: leftButtonEvent.move(event); break;
            default: otherButtonPressedEvent.move(event); break;
        }
    });
    window.addEventListener("mouseup", (event)=>{
        event.preventDefault();

        if(buttonMousePressed != event.button) return;
        
        switch(event.button){
            case KEY_MOUSE.RIGHT_BUTTON: rightButtonEvent.up(event); break;
            case KEY_MOUSE.LEFT_BUTTON: leftButtonEvent.up(event); break;
            default: otherButtonPressedEvent.up(event); break;
        }
        
        buttonMousePressed = undefined;
    });
    window.addEventListener("mouseleave", ()=>{
    });
    window.addEventListener("mousewheel", (event)=>{
        event.preventDefault()
    }, { passive: false });

    canvas.addEventListener("touchleave", (event)=>{
    });
    canvas.addEventListener("touchstart", (event)=>{
        event.preventDefault();
        if(event.touches.length == 1){
            touchTimer = setTimeout(() => {
                singleTouchEvent.down(event);
                touchID = event.touches[0].identifier;
            }, 40);
        }
        else if(event.touches.length == 2){
            clearTimeout(touchTimer);
            singleTouchEvent.up(event);
            touchID = undefined;

            doubleTouchEvent.down(event);
        } else 
            return;
    });
    canvas.addEventListener("touchmove", function (event){
        if(event.touches.length == 1 && event.changedTouches[0].identifier == touchID){
            singleTouchEvent.move(event);
        } else if(event.touches.length == 2){
            doubleTouchEvent.move(event);
        } else 
            return;
    });
    window.addEventListener("touchend", (event)=>{
        if(event.touches.length == 1)
            doubleTouchEvent.up(event);

        for(let i = 0; i < event.changedTouches.length; i++){
            if(event.changedTouches[i].identifier == touchID){
                singleTouchEvent.up(event);
                touchID = undefined;
            }
        }
    });
    window.addEventListener("touchcancel", (event)=>{
        singleTouchEvent.up(event);
    });
    window.addEventListener("mouseleave", ()=>{
        buttonMousePressed = undefined;
    });
    window.addEventListener("mousewheel", (event)=>{
        event.preventDefault()
    }, { passive: false });


    window.oncontextmenu = function() { return false; };
        
    function cursorToPixel(point, middlePoint=false){
        let position ={
            x: canvas.offsetLeft,
            y: canvas.offsetTop
        }

        let pixel = {
            x: (point.x - position.x) / targetScale,
            y: (point.y - position.y) / targetScale
        }
        if(middlePoint){
            pixel.x = Math.floor(pixel.x + 0.5);
            pixel.y = Math.floor(pixel.y + 0.5);
            return pixel;
        }
        pixel.x = Math.floor(pixel.x);
        pixel.y = Math.floor(pixel.y);

        return pixel;
    }
        
    return Object.seal({
        setSingleTouchEvent,
        setDoubleTouchEvent,
        setClickRightEvent,
        setClickLeftEvent,
        setRightButtonMousePressedEvent,
        setLeftButtonMousePressedEvent,
        setGenericButtonMousePressedEvent,
        setScrollEvent,
        setMoveEvent,
        preventDefaultMoveEvent: ()=>{
            preventDefaultMoveEvent = false
        },
        unPreventDefaultMoveEvent: ()=>{
            preventDefaultMoveEvent = false
        }
    });
}