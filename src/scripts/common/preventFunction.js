

export function preventFunction(callback){
    let flagIsRunning = false;
    return async (...args)=>{
        if(flagIsRunning){
            return;   
        }
        
        flagIsRunning = true;
        await callback.apply(this, args);
        flagIsRunning = false;
    }
}