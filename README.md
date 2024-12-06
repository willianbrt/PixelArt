# PixelArt

## 📂 Estrutura
```plaintext
src
├── resource                # Arquivos estáticos como imagens, fontes, etc.
    ├── font
    ├── icons
    ├── img
├── scripts                 # Scripts JavaScript
    ├── common              # Scripts JavaScript reutilizáveis
├── services                # Acesso a APIs
    ├── vendor              # APIs externas
├── styles                  # Botões, inputs, checkbox, etc.
    ├── components          # Botões, inputs, checkbox, etc.
    ├── layouts             # Footer, Navbar, etc.
    ├── vendor              # Bibliotecas externas
    ├── global.css          # Styles apicaveis a todas as paginas  
├── app                     # Páginas HTML e suas dependências
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
