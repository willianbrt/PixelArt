import { rotate, getDelta } from "./dot.js"

// INPUTS
let inpColorHex = document.querySelector("input[name=hex]");

let inpColorR = document.querySelector("input[name=r]");
let inpColorG = document.querySelector("input[name=g]");
let inpColorB = document.querySelector("input[name=b]");

let inpColorH = document.querySelector("input[name=h]");
let inpColorS = document.querySelector("input[name=s]");
let inpColorL = document.querySelector("input[name=l]");
    
inpColorR.addEventListener("input", updateRGB);
inpColorG.addEventListener("input", updateRGB);
inpColorB.addEventListener("input", updateRGB);

inpColorH.addEventListener("input", updateHSL);
inpColorS.addEventListener("input", updateHSL);
inpColorL.addEventListener("input", updateHSL);

inpColorHex.addEventListener("input", updateHex);

let modalChromatic = chromatic({
    content: document.getElementById("chromatic"),
    width:100,
    height:100,
    onUpdateColor: onUpdateColor
});

function updateHex(){
    try{
        let color = ColorFactory().buildByHex(this.value);
        modalChromatic.setColorByHue(color.hsl.h, color.hsl.s, color.hsl.l);
    } catch(e){
        console.log(e)
    }
}
function updateHSL(){
    if(inpColorH.value < 0){
        inpColorH.value = 359;
    }else if(inpColorH.value >= 360){
        inpColorH.value = 0;
    }
    inpColorS.value = Math.min(100, Math.max(inpColorS.value, 0));
    inpColorB.value = Math.min(100, Math.max(inpColorB.value, 0));

    let h = Number(inpColorH.value);
    let s = Number(inpColorS.value);
    let l = Number(inpColorL.value);

    let color = ColorFactory().buildByHSL(inpColorH.value, inpColorS.value, inpColorL.value);

    inpColorHex.value = color.hex.replace(/^#/,"");

    inpColorR.value = color.rgb.r;
    inpColorG.value = color.rgb.g;
    inpColorB.value = color.rgb.b;

    modalChromatic.setColorByHue(h,s,l);
}
function updateRGB(){
    inpColorR.value = Math.min(255, Math.max(inpColorR.value, 0));
    inpColorG.value = Math.min(255, Math.max(inpColorG.value, 0));
    inpColorB.value = Math.min(255, Math.max(inpColorB.value, 0));

    let color = ColorFactory().buildByRGB(inpColorR.value, inpColorG.value, inpColorB.value);
    
    inpColorHex.value = color.hex.replace(/^#/,"");

    inpColorH.value = Math.round(color.hsl.h);
    inpColorS.value = color.hsl.s;
    inpColorL.value = color.hsl.l;

    modalChromatic.setColorByHue(color.hsl.h, color.hsl.s, color.hsl.l);
}
function onUpdateColor(hue, saturation, lightness){
    let color = ColorFactory().buildByHSL(hue, saturation, lightness);

    inpColorHex.value = color.hex.replace(/^#/,"");

    inpColorR.value = color.rgb.r;
    inpColorG.value = color.rgb.g;
    inpColorB.value = color.rgb.b;

    inpColorH.value = Math.round(color.hsl.h);
    inpColorS.value = color.hsl.s;
    inpColorL.value = color.hsl.l;

    let selectedColor = document.querySelector(".work-color.primary-color");
    selectedColor.style.background = color.hex;

    let newColor = document.querySelector("#new-color");
    newColor.style.background = color.hex;
}

function chromatic(options){
    const RAD_TO_DEG = 180 / Math.PI;
    const DEG_TO_RAD = Math.PI / 180;

    let _hue = 0;
    let _saturation = 100;
    let _lightness = 50;

    const canvas = document.getElementById("color-picker");
    const ctx = canvas.getContext("2d", { willReadFrequently: true });

    // TAMANHO PICKER
    const width = canvas.width;
    const height = canvas.height;

    const padding = 5;
    const cx = width / 2;
    const cy = height / 2;
    const outerRadius = Math.min(height, width)/2;
    const strokeWidth = 10;
    const innerRadius = outerRadius - strokeWidth;
    const centerRadius = innerRadius + strokeWidth/2;
    
    // MARCADORES
    let hueMarker = document.querySelector(".marker#hue");
    let colorMarker = document.querySelector(".marker#color");
    let chromatic = buildChromatic(innerRadius-padding, {x: cx,y:cy}, 90);
    
    (()=>{
        setColorByHue(_hue, _saturation,_lightness);
        drawColorWheel();
        drawChromaticTriangle();
    })();

    hueMarker.addEventListener("mousedown", onUpdateHue);
    colorMarker.addEventListener("mousedown", onUpdateColor);
    
    canvas.addEventListener("mousedown", function(e){
        e.preventDefault();
        let cursorX = e.clientX - canvas.getBoundingClientRect().left;
        let cursorY = e.clientY - canvas.getBoundingClientRect().top;

        const distance = Math.sqrt((cursorX - cx) ** 2 + (cursorY - cy) ** 2);

        if(distance >= innerRadius && distance <= outerRadius)
        {
            onUpdateHue(e);
        }
        // else if(chromatic.isInside(chromatic.getBarycentric({x:cursorX, y:cursorY})))
        else if(chromatic.isInside({x:cursorX, y:cursorY}))
        {
            onUpdateColor(e);
        }
    });

    function onUpdateColor(e){
        e.preventDefault();
        let abort = new AbortController();
        const rect = canvas.getBoundingClientRect();

        let x = e.clientX - rect.left;
        let y = e.clientY - rect.top;
        
        setColorByPoint(x, y);
        
        window.addEventListener("mousemove", function(event){
            event.preventDefault();

            x = event.clientX - rect.left;
            y = event.clientY - rect.top;
            
            setColorByPoint(x, y);
        }, { signal: abort.signal });

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("blur", ()=>{abort.abort()}, {once:true}); 
    }
    function onUpdateHue(e){
        e.preventDefault();
        
        let abort = new AbortController();
        const rect = canvas.getBoundingClientRect();
    
        let cursorX = e.clientX - rect.left;
        let cursorY = e.clientY - rect.top;
        setHueByPosition(cursorX, cursorY);

        window.addEventListener("mousemove", function(ev){
            ev.preventDefault();

            cursorX = ev.clientX - rect.left;
            cursorY = ev.clientY - rect.top;
            setHueByPosition(cursorX, cursorY);
            
            options?.onUpdateColor(_hue, _saturation, _lightness);
        }, { signal: abort.signal });

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
    }
    function setHueByPosition(x, y){
        const dx = x - cx;
        const dy = y - cy;
        const rad = Math.atan2(dy, dx);
        const degree = (rad * RAD_TO_DEG + 360) % 360;

        const markerX = Math.cos(rad) * centerRadius + cx;
        const markerY = Math.sin(rad) * centerRadius + cy;

        hueMarker.style.top = `${markerY - hueMarker.offsetHeight/2}px`;
        hueMarker.style.left = `${markerX - hueMarker.offsetWidth/2}px`;
        
        _hue = degree;
        chromatic.setColor(_hue);

        requestAnimationFrame(() => drawChromaticTriangle());
    }
    function drawChromaticTriangle(){
        const imageData = ctx.getImageData(chromatic.bounding.min.x, chromatic.bounding.min.y, chromatic.bounding.width, chromatic.bounding.height);
        const data = imageData.data;

        for (let y = chromatic.bounding.min.y; y < chromatic.bounding.max.y; y++) {
            for (let x = chromatic.bounding.min.x; x < chromatic.bounding.max.x; x++) {
                try {
                    const {r, g, b} = chromatic.getColor({ x, y }).rgb;

                    const index = ((y-chromatic.bounding.min.y) * (chromatic.bounding.width) + (x-chromatic.bounding.min.x)) * 4;
                    data[index] = r;
                    data[index + 1] = g;
                    data[index + 2] = b;
                    data[index + 3] = 255;
                } catch(e){
                    continue;
                }
            }
        }
        ctx.clearRect(chromatic.bounding.min.x, chromatic.bounding.min.y, chromatic.bounding.width, chromatic.bounding.height);
        ctx.putImageData(imageData, chromatic.bounding.min.x, chromatic.bounding.min.y);
    }
    
    function setColorByPoint(x, y){
        let colorMarkerPosition = chromatic.clampped(x, y);
        const color = chromatic.getColor(colorMarkerPosition);
        _saturation= color.hsl.s;
        _lightness = color.hsl.l;

        colorMarker.style.left = `${(colorMarkerPosition.x - colorMarker.offsetWidth/2)}px`;
        colorMarker.style.top = `${(colorMarkerPosition.y - colorMarker.offsetHeight/2)}px`;

        options?.onUpdateColor(_hue, _saturation, _lightness);
    }
    function setColorByHue(hue, saturation, lightness){
        console.log(hue, saturation, lightness);
        try{
            const position = chromatic.getPositionColor(saturation, lightness);

            colorMarker.style.left = `${(position.x - colorMarker.offsetWidth / 2)}px`;
            colorMarker.style.top = `${(position.y - colorMarker.offsetHeight / 2)}px`;

            const rad = hue * DEG_TO_RAD;
            
            const markerX = Math.cos(rad) * centerRadius + cx;
            const markerY = Math.sin(rad) * centerRadius + cy;

            hueMarker.style.top = `${markerY - hueMarker.offsetHeight/2}px`;
            hueMarker.style.left = `${markerX - hueMarker.offsetWidth/2}px`;

            _hue = hue;
            _saturation = saturation;
            _lightness = lightness;
            chromatic.setColor(_hue);
            requestAnimationFrame(() => drawChromaticTriangle());
        } catch(e){
            console.log(e)
        }
        options?.onUpdateColor(hue, saturation, lightness);
    }
    
    function drawColorWheel(){
        let gradient = ctx.createConicGradient(0, cx, cy);
        let endColor = 0;

        const INCREMENT_END_COLOR = 1/360;
        for (let angle = 0; angle < 360; angle += 1) {
            gradient.addColorStop(endColor, `hsl(${angle}, 100%, 50%)`);
            endColor += INCREMENT_END_COLOR;
        }

        ctx.beginPath();
        ctx.lineWidth = strokeWidth;
        ctx.strokeStyle = gradient;
        ctx.arc(cx, cy, outerRadius-strokeWidth/2, 0, 2 * Math.PI);
        ctx.stroke();
        ctx.closePath();
    }
    function buildTriangle(radius, centerPosition, rotateDeg){
        const lado = Math.sqrt(3) * (radius);
        const altura = Math.sqrt(3)*lado/2;
        let _hue = 0;
        let targetColor = ColorFactory().buildByHSL(0, 100, 50);

        let vertexA = { x: centerPosition.x - lado/2, y: centerPosition.y + (radius)*Math.sin(30*Math.PI/180) }; 
        let vertexB = { x: vertexA.x + lado, y: vertexA.y};
        let vertexC = { x: centerPosition.x, y: vertexA.y - altura };
    
        let centroide = {
            x: (vertexA.x + vertexB.x + vertexC.x) / 3,
            y: (vertexA.y + vertexB.y + vertexC.y) / 3,
        };
        
        rotate(vertexA, rotateDeg, centroide);
        rotate(vertexB, rotateDeg, centroide);
        rotate(vertexC, rotateDeg, centroide);
    
        let detABC = determinant(vertexA, vertexB, vertexC);

        function setColor(hue) {
            _hue = hue;
            targetColor = ColorFactory().buildByHSL(hue, 100, 50);
        }

        function getPositionColor(h, s, l) {
            setColor(h);
            s/=100;
            l/=100;

            const puro = s;
            const branco = l - 0.5 * s;
            const preto = 1 - branco - puro;

            const x = preto * vertexB.x + branco * vertexA.x + puro * vertexC.x;
            const y = preto * vertexB.y + branco * vertexA.y + puro * vertexC.y;

            return clampped(x, y);
        }

        function getColor(point){
            const [ lambdaA, lambdaB, lambdaC] = getBarycentric(point);
            if (!isInside([ lambdaA, lambdaB, lambdaC])){
                throw new Error("Point out of bounds");
            }

            const brightness = 255*lambdaB;
            const r = Math.max(0,Math.min(255, Math.round(targetColor.rgb.r*lambdaA + brightness)));
            const g = Math.max(0,Math.min(255, Math.round(targetColor.rgb.g*lambdaA + brightness)));
            const b = Math.max(0,Math.min(255, Math.round(targetColor.rgb.b*lambdaA + brightness)));
            
            const edge = Math.min(lambdaA, lambdaB, lambdaC);
            const smoothingThreshold = 0.018;
            const a = 255 * smoothstep(0, smoothingThreshold, edge);

            return [r,g,b,a];
        }

        function getBarycentric(point){
            let detABP = determinant(vertexA, vertexB, point);
            let detBCP = determinant(vertexB, vertexC, point);
            let detCAP = determinant(vertexC, vertexA, point);
    
            const alpha = detABP / detABC;
            const beta = detBCP / detABC;
            const gamma = detCAP / detABC ;
    
            return [alpha, beta, gamma];
        }
        
        function determinant(vertexA, vertexB, point){
            const ab = {
                x: vertexB.x - vertexA.x,
                y: vertexB.y - vertexA.y
            };
            const ap = {
                x: point.x - vertexA.x,
                y: point.y - vertexA.y
            };
            return ab.x*ap.y - ab.y*ap.x;
        }
    
        function getBounding(){
            const min = {
                x:  Math.floor(Math.min(vertexA.x, vertexB.x, vertexC.x)),
                y:  Math.floor(Math.min(vertexA.y, vertexB.y, vertexC.y))
            };
            const max = {
                x:  Math.ceil(Math.max(vertexA.x, vertexB.x, vertexC.x)),
                y:  Math.ceil(Math.max(vertexA.y, vertexB.y, vertexC.y))
            };
            return {
                min,
                max,
                width: max.x - min.x,
                height: max.y - min.y
            }
        }
    
        function isInside([alpha, beta, gamma], epsilon = 1e-6){
            const sum = alpha + beta + gamma;
            return (
                alpha >= -epsilon &&
                beta >= -epsilon &&
                gamma >= -epsilon &&
                sum >= 1 - epsilon &&
                sum <= 1 + epsilon
            );
        }
    
        function clampped(x, y) {
            const p = { x, y };
            
            if(chromatic.isInside(getBarycentric({x,y}))) return p;
    
            const ab = closestPointOnSegment(p, vertexA, vertexB);
            const bc = closestPointOnSegment(p, vertexB, vertexC);
            const ca = closestPointOnSegment(p, vertexC, vertexA);
    
            const dAB = distanceSquared(p, ab);
            const dBC = distanceSquared(p, bc);
            const dCA = distanceSquared(p, ca);
    
            if (dAB <= dBC && dAB <= dCA) return ab;
            if (dBC <= dCA) return bc;
            return ca;
        }
    
        function closestPointOnSegment(p, a, b) {
            const abx = b.x - a.x;
            const aby = b.y - a.y;
            const t = ((p.x - a.x) * abx + (p.y - a.y) * aby) / (abx * abx + aby * aby);
    
            const clampedT = Math.max(0, Math.min(1, t));
            
            return {
                x: a.x + clampedT * abx,
                y: a.y + clampedT * aby
            };
        }
    
        function distanceSquared(p1, p2) {
            const dx = p1.x - p2.x;
            const dy = p1.y - p2.y;
            return dx * dx + dy * dy;
        }

        function smoothstep(edge0, edge1, x) {
            const t = Math.min(Math.max((x - edge0) / (edge1 - edge0), 0), 1);
            return t * t * (3 - 2 * t);
        }
        

        return {
            vertexA,
            vertexB,
            vertexC,
            bounding: getBounding(),
            getColor,
            getBarycentric,
            calculateArea: determinant,
            isInside,
            clampped,
            closestPointOnSegment,
            distanceSquared,
            setColor,
            getPositionColor,
        }
    }

    function buildChromatic(radius, centerPosition){
        let colorFactory = ColorFactory();
        let targetColor = colorFactory.buildByHSL(0, 100, 50);

        function setColor(hue) {
            targetColor = colorFactory.buildByHSL(hue, 100, 50);
        }

        function getPositionColor(saturation, lightness) {
            const bounding = getBounding();
            
            const x = saturation/100*bounding.width + bounding.min.x;
            const y = (1-lightness/100)*bounding.height + bounding.min.y;

            return clampped(x, y);
        }

        function getColor(point){
            const bounding = getBounding();
            const u = (point.x - bounding.min.x) / (bounding.width); // 0 a 1 no eixo X
            const v = (point.y - bounding.min.y) / (bounding.height); // 0 a 1 no eixo Y

            const w0 = (1 - u) * (1 - v); // canto superior esquerdo
            const w1 = u * (1 - v);       // canto superior direito
            const w2 = u * v;             // canto inferior direito
            const w3 = (1 - u) * v;       // canto inferior esquerdo

            const h = Math.round(targetColor.hsl.h);
            const s = Math.round(100*w1 + 100*w2);
            const l = Math.round(100*w0 + 100*w1);

            return colorFactory.buildByHSL(h,s,l);
        }

        function isInside({x, y}){
            const bounding = getBounding();
            return x >= bounding.min.x && x <= bounding.max.x
                && y >= bounding.min.y && y <= bounding.max.y;
        }
    
        function clampped(x, y) {
            const bounding = getBounding();

            return{
                x: Math.max(bounding.min.x, Math.min(bounding.max.x, x)),
                y: Math.max(bounding.min.y, Math.min(bounding.max.y, y))
            }
        }

        function getBounding(){
            const offsetCenter = (radius * Math.sqrt(2)) / 2;
            const min = {
                x:  Math.floor(centerPosition.x - offsetCenter),
                y:  Math.floor(centerPosition.y - offsetCenter)
            };
            const max = {
                x:  Math.floor(centerPosition.x + offsetCenter),
                y:  Math.floor(centerPosition.y + offsetCenter)
            };
            return {
                min,
                max,
                width: max.x - min.x,
                height: max.y - min.y
            }
        }
        return {
            bounding: getBounding(),
            getColor,
            isInside,
            clampped,
            setColor,
            getPositionColor,
        }
    }

    return {
        setColorByHue
    }
}

function ColorFactory(){
    function buildByRGB(r,g,b){ 
        return {
            rgb: [r, g, b],
            hsl: rgbToHsl(r, g, b),
            hex: rgbToHex(r, g, b)
        }
    }
    function buildByHSL(h,s,l){
        const {r,g,b} = hslToRgb(h,s,l);

        return {
            rgb: {r,g,b},
            hsl: { h, s, l},
            hex: rgbToHex(r,g,b)
        };
    }
    function buildByHex(hex){
        const {r,g,b} = hexToRgb(hex);

        return {
            rgb: {r,g,b},
            hsl: rgbToHsl(r, g, b),
            hex: hex
        }
    }
    
    function rgbToHex(r, g, b) {
        return "#" + [r, g, b]
            .map(x => {
                const hex = x.toString(16);
                return hex.length === 1 ? "0" + hex : hex;
            })
            .join("");
    }
    function rgbToHsl(r, g, b) {
        r /= 255;
        g /= 255;
        b /= 255;

        const max = Math.max(r, g, b);
        const min = Math.min(r, g, b);
        const delta = max - min;

        let h = 0, s = 0, l = (max + min) / 2;

        if (delta !== 0) {
            s = delta / (1 - Math.abs(2 * l - 1));

            switch (max) {
                case r:
                    h = ((g - b) / delta) % 6;
                    break;
                case g:
                    h = (b - r) / delta + 2;
                    break;
                case b:
                    h = (r - g) / delta + 4;
                    break;
            }

            h *= 60;
            if (h < 0) h += 360;
        }

        return {
            h: Math.round(h),
            s: +(s * 100).toFixed(0),
            l: +(l * 100).toFixed(0)
        };
    }
    function hexToRgb(hex) {
        hex = hex.replace(/^#/, "");
        if (!(/^[0-9A-Fa-f]{6}$/.test(hex)) && !(/^[0-9A-Fa-f]{3}$/.test(hex))) {
            throw new Error("Fora do escopo");
        }
        if (hex.length === 3) {
            hex = hex.split("").map(c => c + c).join("");
        }

        const bigint = parseInt(hex, 16);
        const r = (bigint >> 16) & 255;
        const g = (bigint >> 8) & 255;
        const b = bigint & 255;

        return { r, g, b };
    }
    function hslToRgb(h, s, l) {
        s /= 100;
        l /= 100;

        const c = (1 - Math.abs(2 * l - 1)) * s;
        const x = c * (1 - Math.abs((h / 60) % 2 - 1));
        const m = l - c / 2;

        let r = 0, g = 0, b = 0;

        if (0 <= h && h < 60) {
            [r, g, b] = [c, x, 0];
        } else if (60 <= h && h < 120) {
            [r, g, b] = [x, c, 0];
        } else if (120 <= h && h < 180) {
            [r, g, b] = [0, c, x];
        } else if (180 <= h && h < 240) {
            [r, g, b] = [0, x, c];
        } else if (240 <= h && h < 300) {
            [r, g, b] = [x, 0, c];
        } else if (300 <= h && h < 360) {
            [r, g, b] = [c, 0, x];
        }

        r = Math.round((r + m) * 255);
        g = Math.round((g + m) * 255);
        b = Math.round((b + m) * 255);

        return { r, g, b };
    }

    return {
        buildByRGB,
        buildByHSL,
        buildByHex
    }
}