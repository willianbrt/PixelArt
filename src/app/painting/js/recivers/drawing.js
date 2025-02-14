import Graphics from "../build/graphics/graphics.js";

export default async function Sketch({
    canvas,
    width,
    height
}){
    if(!isValidCanvas()) throw new Error("Objeto 'canvas' não encontrado.");
    if(!isValidSize()) throw new Error("Os parâmetros 'sketchWidth' e 'sketchHeight' devem conter uma valor entre 0 e 1200.");

    
    const project = await Graphics({ canvas: canvas });

    let scene = new project.Scene(10, 10);
    scene.render();
    scene.addLayer("");
    scene.remove("");
    

    
    return {
        resize,
        panning,
        scroll:{
            zoomIn,
            zoomOut,
        },
        pressed:{
            drawline,
            drawcircle,
            blending,
            drawrect,
            putpixel,
            spray,
            cut,
        },
        getScale,
        getMinScale, getMaxScale,
        getAllFrames,
        get,
    }
};