import ICommand from "../interfaces/ICommand.js";

export default function History(){
    const MAX_ACTIONS = 26;
    let historyQueue = [MAX_ACTIONS];
    let redoQueue = [MAX_ACTIONS];
    
    var execute = (command)=>{
        if(!isValidCommand(command)) throw new Error("Commando inválido");

        command.execute();
        
        if(historyQueue.length == MAX_ACTIONS){
            historyQueue.unshift();
        }
        
        historyQueue.push(command);
        redoQueue = [MAX_ACTIONS];
    };

    var undo = ()=>{
        if(historyQueue.length == 0){ return; }

        let history = historyQueue.pop();
        history.command.undo(history.args);
        
        redoQueue.push(history);
    };

    var redo = ()=>{
        if(redoQueue.length == 0){ return; }

        command.excute();
        
        var history = redoQueue.pop();
        history.command.execute(_layer);
    };
    let isValidCommand = command => command instanceof ICommand;

    return Object.seal({
        execute,
        undo,
        redo
    });
}