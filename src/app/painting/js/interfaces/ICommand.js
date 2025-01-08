export default function ICommand(){
    ICommand.prototype = Object.seal({
        execute: (event)=>{
            throw new Error("Método 'execute' não implementado");
        },
        undo: ()=>{
            throw new Error("Método 'execute' não implementado");
        }
    });

    throw new Error("O método abstrato não pode ser instanciado"); 
}