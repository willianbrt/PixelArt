export function Shortcuts(){
    let _shortcuts = new Map();
    let _activeScope = new Set(["global","globaal"]);
    
    window.addEventListener("keydown", function(event){
        const isContentsEditable =  document.activeElement.tagName === "INPUT" ||
        document.activeElement.tagName === "TEXTAREA" ||
        document.activeElement.isContentEditable;
        
        if(isContentsEditable && event.keyCode == 27){ document.activeElement.blur(); }
        
        if(isContentsEditable
            && (!event.ctrlKey
            || (event.ctrlKey && [67,65,86,88,89,90].indexOf(event.keyCode) > -1))
         ){
             return;
        }
        // event.preventDefault();
        
        let keycombo = normalize({
            shift:event.shiftKey,
            ctrl:event.ctrlKey,
            alt: event.altKey,
            keyCode: event.keyCode
        });
        invoke(keycombo);
    });

    function invoke(keycombo){
        if(!_shortcuts.has(keycombo))return;
        
        _shortcuts.get(keycombo).map((shortcut)=>{
            if(!_activeScope.has(shortcut.scope)) return;
                shortcut.callback();
        });
    }
    function normalize({
            shift,
            ctrl,
            alt,
            keyCode
        }){
        let combo = [];
        if(keyCode != 17 && ctrl){ combo.push("ctrl"); }
        if(keyCode != 16 && shift){ combo.push("shift"); }
        if(keyCode != 18 && alt){ combo.push("alt"); }

        combo.push(keyCode);

        return combo.join("+");
    }
    function register({
        default: {
            shift,
            ctrl,
            alt,
            keyCode
        },
        description,
        scope,
        callback
    }){
        const combo = normalize({
            shift,
            ctrl,
            alt,
            keyCode
        });
        let shortcut = _shortcuts.get(combo) ?? [];
        if(shortcut.length > 0){
            let index = shortcut.findIndex(s => s.scope === scope);
            if(index > -1){
                shortcut[index] = {
                    description,
                    scope,
                    callback
                };
            } else {
                shortcut.push({
                    description,
                    scope,
                    callback
                });
            }
            return;
        }
        
        _shortcuts.set(combo, [{
            description,
            scope,
            callback
        }]);
    }
    function disableScope(scope){
        _activeScope.delete(scope);
    }
    function activeScope(scope){
        if(!_activeScope.has(scope))
            _activeScope.add(scope);
    }

    return {
        register,
        invoke,
        disableScope,
        activeScope
    }
}