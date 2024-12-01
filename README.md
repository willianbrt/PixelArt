# PixelArt

## 📂 Estrutura
```plaintext
src
├── resource
    ├── font
    ├── icons
    ├── img
├── scripts
    ├── common
    ├── services
    ├── vendor
├── styles
    ├── components
    ├── layouts
    ├── vendor
    ├── global.css 
├── view
```


## Primeiros Passos

### Configuração do ambiente
```plaintext
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

git pull

emsdk install latest
emsdk activate --permanent
```
**Mais detalhes em:**
Como Instalar Emscripten: <https://developer.mozilla.org/en-US/docs/WebAssembly/C_to_Wasm>
WebAssembly Javascript: <https://developer.mozilla.org/en-US/docs/WebAssembly/JavaScript_interface>

### Hello Word

```plaintext
emcc index.c -o index.js
```
