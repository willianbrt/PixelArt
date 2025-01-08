export function IHover(){
    IHover.prototype = Object.seal({
        moving: (event)=>{
            throw new Error("Método 'mousemove' não implementado");
        }   
    });

    throw new Error("O método abstrato não pode ser instanciado"); 
}

export function IPressed(){
    IPressed.prototype = Object.seal({
        pressed: (event)=>{
            throw new Error("Método 'mousedown' não implementado");
        },
        released: (event)=>{
            throw new Error("Método 'mouseup' não implementado");
        },
        tracking: (event)=>{
            throw new Error("Método 'mousemove' não implementado");
        }
    });

    throw new Error("O método abstrato não pode ser instanciado"); 
}

export function IScroll(){
    IScroll.prototype = Object.seal({
        scroll: (event)=>{
            throw new Error("Método 'scroll' não implementado");
        }
    });

    throw new Error("O método abstrato não pode ser instanciado"); 
}


export function IClick(){
    IClick.prototype = Object.seal({
        click: (event)=>{
            throw new Error("Método 'click' não implementado");
        },
    });

    throw new Error("O método abstrato não pode ser instanciado"); 
}