import { app } from "./app.js"
import { modal } from "./elements.js"
import { Chromatic, ColorFactory } from "./chromatic.js"


const factoryColor = ColorFactory();
let secondaryColor;

let selectColorMode = document.querySelector("select[name='color-pattern']");
selectColorMode.addEventListener("change", loadColorMode);

let workPrimaryColor = document.querySelector(".work-color.primary-color");
let workSecondaryColor = document.querySelector(".work-color.secondary-color");
workSecondaryColor.onclick = (e)=>{ swapActiveColor(secondaryColor); };

let btnAddColor= document.querySelector("#add-color");
btnAddColor.onclick = createColor;

let selectPalette = document.querySelector("select[name='palette']");
selectPalette.addEventListener("change", loadActivePalette);

let listColors= document.getElementById("list-colors");

// INPUTS
let inpColorHex = document.querySelector("input[name=hex]");
inpColorHex.addEventListener("input", updateHEX);

let inpColorR = document.querySelector("input[name=r]");
let inpColorG = document.querySelector("input[name=g]");
let inpColorB = document.querySelector("input[name=b]");
[inpColorR, inpColorG, inpColorB].forEach(e=>e.addEventListener("input", updateRGB));

let inpColorH = document.querySelector("input[name=h]");
let inpColorS = document.querySelector("input[name=s]");
let inpColorL = document.querySelector("input[name=l]");
[inpColorH, inpColorS, inpColorL].forEach(e=>e.addEventListener("input", updateHSL));

let modalPaletteName = modal({modalId: "modal-palette-name", triggerId: "new-palette"});

let btnAddPalette = document.querySelector("#add-palette");
btnAddPalette.onclick = createPalette;

let btnRemovePalette = document.querySelector("#trash-palette");
btnRemovePalette.onclick = removePalette;
    
let paletteRepository, modalChromatic;
let drawingSettings;
export async function buildPanePalette(){
    drawingSettings = app.drawingSettingsVM();

    [paletteRepository, modalChromatic] = await Promise.all([
        PaletteRepository(),
        Chromatic({
            content: document.getElementById("chromatic"),
            width:100,
            height:100,
            onUpdateColor: (color)=>{
                inpColorHex.value = color.hex.replace(/^#/,"");

                inpColorR.value = color.rgb.r;
                inpColorG.value = color.rgb.g;
                inpColorB.value = color.rgb.b;

                inpColorH.value = Math.round(color.hsl.h);
                inpColorS.value = color.hsl.s;
                inpColorL.value = color.hsl.l;
                
                changePrimaryColor(color);
            }
        })
    ]);
    
    changeSecondaryColor(factoryColor.buildByRGB(255,255,255));
    loadColorMode();
    
    const listPalette = await paletteRepository.getAllPalette();
    listPalette.forEach(p => loadPalette(p));
    loadActivePalette();
}
async function loadColorMode(){
    document.querySelectorAll(".color-form").forEach((f)=>f.classList.toggle("hidden", true));
    let modes = {
        "HEX": ()=> document.querySelector("#inp-hex"),
        "RGB": ()=> document.querySelector("#inp-rgb"),
        "HSL": ()=> document.querySelector("#inp-hsl"),
    };
    modes[selectColorMode.value]().classList.toggle("hidden", false);
}
function loadPalette(p){
    let optionPalette = document.createElement("option");
    optionPalette.value = p.id;
    optionPalette.innerText = p.name;
    selectPalette.append(optionPalette)
}
async function loadActivePalette(){
    listColors.querySelectorAll(".color").forEach((c)=>c.remove());
    let activePalette = await paletteRepository.getPalette(parseInt(selectPalette.value));
    activePalette.colors.forEach((color)=>addColorElement(factoryColor.buildByRGB(color[0], color[1], color[2])));
}
function addColorElement(color){
    let spanColor = listColors.querySelector(`.color[data-color='${color.hex}']`);

    if(spanColor !== null) return;

    spanColor = document.createElement("span");
    spanColor.style.background = color.hex;
    spanColor.className = "color";

    spanColor.dataset.color = color.hex;
    listColors.append(spanColor);
    
    if(drawingSettings.getColor() ==  color.littleEndian){
        swapActiveColor(color);
    }
    spanColor.addEventListener("click", function(){
        swapActiveColor(color);
    });
}
function swapActiveColor(color){
    const currentColor = factoryColor.buildByDecimal(drawingSettings.getColor());
    changePrimaryColor(color);
    changeSecondaryColor(currentColor);
}
function changePrimaryColor(color){
    drawingSettings.setColor(color.rgb.r, color.rgb.g, color.rgb.b);
    
    let newColor = document.querySelector("#new-color");
    workPrimaryColor.style.background = color.hex;
    newColor.style.background = color.hex;

    document.querySelector(".color.active")?.classList.remove("active");
    document.querySelector(`.color[data-color="${color.hex}"]`)?.classList.toggle("active",true);
}
function changeSecondaryColor(color){
    if(drawingSettings.getColor() == color.littleEndian){ return; }

    secondaryColor = color;

    let oldColor = document.querySelector("#old-color");
    workSecondaryColor.style.background = secondaryColor.hex;
    oldColor.style.background = secondaryColor.hex;
}
function createPalette(){
    try{
        let inpNamePalette = document.querySelector("input[name='name-palette']");
        let namePalette = inpNamePalette.value;
        paletteRepository.createPalette(namePalette)

        let paletteOption = document.createElement("option");
        paletteOption.value = namePalette;
        paletteOption.innerText = namePalette;
        selectPalette.append(paletteOption);

        selectPalette.value = namePalette;
        loadActivePalette();
        inpNamePalette.value = "";

        modalPaletteName.close();
    } catch(e){
        console.error(e);
    }
}
function removePalette(){
    paletteRepository.removePalette(selectPalette.value)
    selectPalette.querySelector(`option[value=${selectPalette.value}]`).remove();
    selectPalette.value = "Default";
    loadActivePalette();
}

async function createColor(){
    const color = factoryColor.buildByDecimal(drawingSettings.getColor());
    
    const activePalette = await paletteRepository.getPalette(parseInt(selectPalette.value));
    if(activePalette.colors.find((c)=> c[0] == color.rgb.r && c[1] == color.rgb.g && c[2] == color.rgb.b)){
        return;
    }
    activePalette.addColor(color);
    
    addColorElement(color);
    swapActiveColor(color);
}

function updateHEX(){
    try{
        let color = factoryColor.buildByHex(this.value);
        modalChromatic.setColor(color);
    } catch(e){
        console.warn(e)
    }
}
function updateRGB(){
    inpColorR.value = Math.min(255, Math.max(inpColorR.value, 0));
    inpColorG.value = Math.min(255, Math.max(inpColorG.value, 0));
    inpColorB.value = Math.min(255, Math.max(inpColorB.value, 0));

    let color = factoryColor.buildByRGB(inpColorR.value, inpColorG.value, inpColorB.value);
    modalChromatic.setColor(color);
}
function updateHSL(){
    if(inpColorH.value < 0){
        inpColorH.value = 359;
    }else if(inpColorH.value >= 360){
        inpColorH.value = 0;
    }
    inpColorS.value = Math.min(100, Math.max(inpColorS.value, 0));
    inpColorL.value = Math.min(100, Math.max(inpColorL.value, 0));

    let color = factoryColor.buildByHSL(inpColorH.value, inpColorS.value, inpColorL.value);
    modalChromatic.setColor(color);
}

async function PaletteRepository(){
    let table = await app.database.table("palette");

    let palette = {
        createPalette: async function(name, colors = [], is_system){
            table.put({name, colors, is_system});
        },
        removePalette: async function(name){
            const elem = await table.findBy("name", name);
            if(elem.is_system)
                throw { source: "[PalletFactory]", description: "Object of System."};

            table.delete(elem.id);
        },
        getPalette: async function(id){
            return Object.assign(await table.getById(id), {
                getColor: function(color){
                    let index = this.colors.findIndex(color);
                    if (index <= -1) return false;
                    return this.colors[index];
                },
                getAllColors: function(){
                    return this.colors;
                },
                addColor: function(color){
                    this.colors.push([color.rgb.r, color.rgb.g, color.rgb.b]);
                    table.put({id: this.id, name: this.name, colors: this.colors, is_system: this.is_system});
                },
                removeColor: function(color){
                    let index = this.colors.findIndex(color);
                    if (index <= -1) return false;
                    this.colors.push(color);
                }
            });
        },
        getAllPalette: async ()=>{ return await table.getAll(); }
    };
    try{
        if(!await table.findBy("name", "Default"))
            palette.createPalette("Default", [[255,0,0], [0,255,12]], true);
        if(!(await table.findBy("name", "Sombras")))
            palette.createPalette("Sombras", [[255,0,0], [50,0,55]], true);
        if(!(await table.findBy("name", "Luzes")))
            palette.createPalette("Luzes", [[50,0,100],[255,0,255]], true);
    } catch(e){
        console.error(e);
    }

    return palette;
}