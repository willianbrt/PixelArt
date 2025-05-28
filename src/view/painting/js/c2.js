import { rotate } from "./dot.js"

export function Chromatic(options){
    const RAD_TO_DEG = 180 / Math.PI;
    const DEG_TO_RAD = Math.PI / 180;

    let _color = ColorFactory().buildByHSL(0, 100, 50);

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
        setColor(_color);
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
            
            options?.onUpdateColor(_color);
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
        
        chromatic.setColor(degree);

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
        _color = chromatic.getColor(colorMarkerPosition);
        
        colorMarker.style.left = `${(colorMarkerPosition.x - colorMarker.offsetWidth/2)}px`;
        colorMarker.style.top = `${(colorMarkerPosition.y - colorMarker.offsetHeight/2)}px`;

        options?.onUpdateColor(_color);
    }
    function setColor(color){
        try{
            _color = color;
            const position = chromatic.getPositionColor(_color.hsl.s, _color.hsl.l);

            colorMarker.style.left = `${(position.x - colorMarker.offsetWidth / 2)}px`;
            colorMarker.style.top = `${(position.y - colorMarker.offsetHeight / 2)}px`;

            const rad = _color.hsl.h * DEG_TO_RAD;
            
            const markerX = Math.cos(rad) * centerRadius + cx;
            const markerY = Math.sin(rad) * centerRadius + cy;

            hueMarker.style.top = `${markerY - hueMarker.offsetHeight/2}px`;
            hueMarker.style.left = `${markerX - hueMarker.offsetWidth/2}px`;

            chromatic.setColor(_color.hsl.h);
            requestAnimationFrame(() => drawChromaticTriangle());
        } catch(e){
            console.warn(e);
        }

        options?.onUpdateColor(_color);
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
        setColor
    }
}

export function ColorFactory(){
    function buildByRGB(r,g,b){ 
        return {
            rgb: {r, g, b},
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
        return "#" + (r << 16 | g << 8 | b).toString(16).padStart(6, "0");
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
            throw new Error("Formato do hexadecimal inválido.");
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