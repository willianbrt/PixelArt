
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

    let createPressedEvent = (strategy)=>{
        resetPointerTracking();

        return {
            down:(event)=>{
                strategy.pressed(event);
                
                canvas.addEventListener("mousemove", (event)=>{
                    event.preventDefault();

                    strategy.tracking(event);
                }, { signal: abortPointerTrackingEvent.signal });
            },
            up:(event)=>{
                resetPointerTracking();
                
                strategy.released(event);
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
        console.log("a")
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
            eventHandler.scroll(event);
        }, { signal: abortScroll.signal });
    }

    function resetScroll(){
        if(abortScroll.signal.aborted) abortScroll.abort();
        abortScroll = new AbortController();
    }

    
    return Object.seal({
        setClickRightEvent,
        setClickLeftEvent,
        setRightButtonMousePressedEvent,
        setGenericButtonMousePressedEvent,
        setScrollEvent
    });
}