import Graphics from "../build/graphics/graphics.js"

(async ()=>{
    const canvas = document.querySelector("#paintingCanvas");

    const ModuleGraphics = await Graphics({
        wasmMemory: new WebAssembly.Memory({initial:256, maximum: 256}),
        canvas:  canvas
    }).then(instance=>{
        let scene = new instance.Scene(10,10);
        scene.render();
        const ctx = canvas.getContext("2d");
        
    });
})();

// mockBlending();

async function mockBlending(){
    const canvas = document.querySelector("#paintingCanvas");
        
    const ModuleGraphics = await Graphics({
        wasmMemory: new WebAssembly.Memory({initial:256, maximum: 256}),
        canvas:  canvas
    }).then(instance=>{
        let scene = new instance.Scene(10,10);
        // scene.render();
        const ctx = canvas.getContext("2d");
        const size = 20;
        const topColor = 0xFF00FF55;
        const bottomColor = 0xFFFF00FF;
    
        const blending = scene.blending(topColor, bottomColor);
        ctx.fillStyle = hexToRgba(blending);
        ctx.fillRect(0,0,size,size);
        
        ctx.fillStyle = hexToRgba(bottomColor);
        ctx.fillRect(size,0,size,size);
        
        ctx.fillStyle = hexToRgba(topColor);
        ctx.fillRect(size,0,size,size);
        
    });
}

function hexToRgba(colorHex){
    let red = colorHex >> 24 & 0xFF;
    let blue = colorHex >> 16 & 0xFF;
    let green = colorHex >> 8 & 0xFF;
    let alpha = colorHex & 0xFF;
    
    return `rgb(${red}, ${blue}, ${green}, ${alpha/255 *100}%)`; 
}


function getAlpha(colorHex){
    let alpha = colorHex & 0xFF;
    return  alpha/255 *100; 
}