# APP

## Primeiros Passos

### Exportar funções

``` sh
emcc ./c/graphics.c -o ./js/graphics.js -s WASM=0 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=[ccall,cwrap]
emcc ./c/graphics.c -o ./wasm/graphics.wasm -s WASM=1 --no-entry -s EXPORTED_FUNCTIONS=["_main"]

emcc ./lib/graphics.c -o ./js/graphics.js -s EXPORTED_FUNCTIONS="['_add']" -s EXPORTED_RUNTIME_METHODS=[ccall,cwrap] -s MODULARIZE=1 -s EXPORT_ES6=1 -s ENVIRONMENT=web

emcc ./lib/graphics.c -o ./build/graphics/graphics.js -s EXPORTED_FUNCTIONS="['_render', '_setSize']" -s EXPORTED_RUNTIME_METHODS=[ccall,cwrap] -s MODULARIZE=1 -s EXPORT_ES6=1 -s ENVIRONMENT=web -s WASM=1 -s STANDALONE_WASM
emcc ./lib/graphics.c -o ./build/graphics/graphics.js -s  EXPORTED_FUNCTIONS='["_render"]'

emcc ./lib/graphics.cpp -o ./build/graphics/graphics.js -s EXPORTED_FUNCTIONS='_render','_setSize' -s ENVIRONMENT=web -s MODULARIZE=1 -s EXPORT_ES6=1 -s IMPORTED_MEMORY
 ```-

~~~c
#include <stdio.h>
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int soma(int first, int second){
    return first + second;
}

EMSCRIPTEN_KEEPALIVE
int main() {
    printf("CONSOLE PELO PRINTF!\n");
    return 0;
}
~~~

~~~html 
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Index - PixelArt</title>

    <script src="index.js"></script>
</head>
<body>
    <button id="somar">Somar</button>

    <script>
        var button = document.querySelector("#somar");
        
        var buttonClicked = button.addEventListener("click", ()=> {
            var result = Module.ccall("main", "number", null, null);
            console.log("CCALL SEM PARAMETRO: " + result);
            
            var soma = Module.ccall("soma", "number", ["number", "number"], [50, 5]);
            console.log("CCALL: " + soma);
            console.log("CWRAP: " + funcaoSoma(60, 70));
        });

        var funcaoSoma = Module.cwrap("soma", "number", ["number", "number"]);
    </script>
</body>
</html>
 ~~~

 
**Atenção!**
Se estiver utilizando o VS Code talvez seja necessário adicionar ao **includePath** o caminho da bilioteca do **emscripten** através do arquivo localizado em: **.vscode/c_cpp_properties.json**.

~~~json
{
    "configurations": [
        {
            "includePath": [
                "C:/Users/User/emsdk/upstream/emscripten/system/include",
            ]
        }
    ],
    "version": 4
}
~~~