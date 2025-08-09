import { PositionHelper } from "../../../scripts/common/position.js";

export default function HandlerEvents(canvas){
    const KEY_MOUSE = {
        RIGHT_BUTTON:0,
        WHELL_BUTTON:1,
        LEFT_BUTTON:2,
    };
    const noEvent = ()=>{
        return {
            down: ()=>{},
            up:()=>{}
        }
    };

    let abortPointerTrackingEvent = new AbortController();
    let abortScroll = new AbortController();
    
    let buttonMousePressed = undefined;

    let leftButtonEvent = noEvent();
    let rightButtonEvent = noEvent();
    let otherButtonPressedEvent = noEvent();
    
    function setGenericButtonMousePressedEvent(strategy){
        otherButtonPressedEvent = createPressedEvent(strategy);
    }
    function setRightButtonMousePressedEvent(strategy){
        rightButtonEvent = createPressedEvent(strategy);
    }
    function setLeftButtonMousePressedEvent(strategy){
        leftButtonEvent = createPressedEvent(strategy);
    }

    let createPressedEvent = (strategy)=>{
        resetPointerTracking();
        return {
            down:(event)=>{
                strategy.onPressed(PositionHelper.getPositionCursor(event));

                canvas.addEventListener("mousemove", (event)=>{
                    requestAnimationFrame(()=>strategy.onTracking(PositionHelper.getPositionCursor(event)));
                }, { signal: abortPointerTrackingEvent.signal });
            },
            up:(event)=>{
                resetPointerTracking();

                requestAnimationFrame(()=>strategy.onRelease(PositionHelper.getPositionCursor(event)));
            }
        }
    } 

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
    

    window.addEventListener("mouseup", (event)=>{
        event.preventDefault();

        if(!(buttonMousePressed == event.button)) return;
        
        switch(event.button){
            case KEY_MOUSE.RIGHT_BUTTON: rightButtonEvent.up(event); break;
            case KEY_MOUSE.LEFT_BUTTON: leftButtonEvent.up(event); break;
            default: otherButtonPressedEvent.up(event); break;
        }
        
        buttonMousePressed = undefined;
    });

    document.addEventListener("mouseleave", ()=>{
        resetPointerTracking();
    });
    
    function setClickLeftEvent(handlerEvent){
        resetPointerTracking();

        leftButtonEvent.down = noEvent();
        leftButtonEvent.up = handlerEvent.click;
    }
    
    function setClickRightEvent(handlerEvent){
        resetPointerTracking();

        rightButtonEvent.down = noEvent();
        rightButtonEvent.up = handlerEvent.click;
    }

    function resetPointerTracking(){
        if(abortPointerTrackingEvent.signal) abortPointerTrackingEvent.abort();
        abortPointerTrackingEvent = new AbortController();
    }

    function setScrollEvent(eventHandler){
        resetScroll();

        canvas.addEventListener("wheel", (event)=>{
            event.preventDefault();
            requestAnimationFrame(()=>eventHandler.onScroll(event.deltaX, event.deltaY, PositionHelper.getPositionCursor(event)));
        }, { signal: abortScroll.signal });
    }

    function resetScroll(){
        if(abortScroll.signal.aborted) abortScroll.abort();
        abortScroll = new AbortController();
    }


    canvas.addEventListener("mousemove", (event)=>{
        event.preventDefault();
        // console.log(event)
        // let cursor = event.getClientRect();
        // let x =  event.clientX - cursor.left;
        // let y =  event.clientY - cursor.top;
        // scene.hover(x, y);
    });

    canvas.addEventListener("mouseleave", (event)=>{
        event.preventDefault();
    });

    
    return Object.seal({
        setClickRightEvent,
        setClickLeftEvent,
        setRightButtonMousePressedEvent,
        setLeftButtonMousePressedEvent,
        setGenericButtonMousePressedEvent,
        setScrollEvent
    });
}