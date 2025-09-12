export let PositionHelper = Object.freeze({
    getPositionCursor: (event,elem)=>{
        let elementPosition = elem.getBoundingClientRect();
        // let elementPosition = event.srcElement.getBoundingClientRect();
        
        return Object.freeze({
            x: Math.floor(event.clientX - elementPosition.left),
            y: Math.floor(event.clientY - elementPosition.top) 
        });
    }
});
