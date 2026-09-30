async function ChromaticRender(canvas){
    const gl = canvas.getContext("webgl2");

    const vs = `#version 300 es
    in vec2 pos;
    out vec2 uv;

    void main() {
        uv = vec2(pos.x,  pos.y);
        gl_Position = vec4(pos, 0, 1);
    }
    `;

    const fs = `#version 300 es
    precision mediump float;

    in vec2 uv;
    out vec4 outColor;
    uniform float hue;
    uniform vec2 res;
    uniform vec2 hueMarkerPosition;
    uniform vec2 colorMarkerPosition;
    uniform vec2 rec;
    uniform float thinknessHUE;

    vec2 p;

    vec3 hslTorgb(float h, float s, float l) {
        float c = (1.0 - abs(2.0*l - 1.0)) * s;
        float x = c * (1.0 - abs(mod(h*6.0, 2.0) - 1.0));
        float m = l - c * 0.5;

        vec3 rgb;

        if (h < 1.0/6.0)      rgb = vec3(c,x,0);
        else if (h < 2.0/6.0) rgb = vec3(x,c,0);
        else if (h < 3.0/6.0) rgb = vec3(0,c,x);
        else if (h < 4.0/6.0) rgb = vec3(0,x,c);
        else if (h < 5.0/6.0) rgb = vec3(x,0,c);
        else                  rgb = vec3(c,0,x);

        return rgb + m;
    }
    float arc(vec2 center, float radius, float thickness){
        vec2 d = p - center ;
        float dist = length(d);

        float halfT = thickness*0.5;
        float edge = fwidth(dist);
        float inner = smoothstep(radius - halfT - edge, radius - halfT + edge, dist);
        float outer = smoothstep(radius + halfT - edge, radius + halfT + edge, dist);

        return inner - outer;
    }

    void main() {
        vec2 center = res*0.5;
        vec2 start = center - (rec*0.5);
        vec2 end = start + rec;
        p = vec2(gl_FragCoord.x, res.y - gl_FragCoord.y);

        if(p.x <= end.x && p.x >= start.x && p.y <= end.y && p.y >= start.y){
            vec2 squareUV = (p - start) / (end - start);

            float s = squareUV.x;
            float l = 1.0-squareUV.y;
            vec3 color = hslTorgb(hue, s, l);
    
            outColor = vec4(color, 1.0);
        }
        vec2 d = center - p;
        float halfT = thinknessHUE*0.5;
        float radius = min(center.x, center.y)-halfT;
        float stroke = arc(center, radius, thinknessHUE);
        
        float angle = atan(d.y, d.x);
        float _hue = (angle / 6.2831853) + 0.5;
        
        outColor = mix(outColor, vec4(hslTorgb(_hue, 1.0, 0.5), 1.0), stroke);
        
        float thinknessMarker = 2.0;
        
        float strokeHueMarker = arc(hueMarkerPosition, thinknessHUE*0.5-thinknessMarker, thinknessMarker);
        outColor = mix(outColor, vec4(1.0), strokeHueMarker);
        
        float strokeColorMarker = arc(colorMarkerPosition, thinknessHUE*0.5-thinknessMarker, thinknessMarker);
        outColor = mix(outColor, vec4(1.0), strokeColorMarker);
    }`;

    function shader(type, src){
        let s = gl.createShader(type);
        gl.shaderSource(s, src);
        gl.compileShader(s);
        return s;
    }
    
    let prog = gl.createProgram();
    gl.attachShader(prog, shader(gl.VERTEX_SHADER, vs));
    gl.attachShader(prog, shader(gl.FRAGMENT_SHADER, fs));
    gl.linkProgram(prog);
    gl.useProgram(prog);
    
    const buf = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buf);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        -1,-1,  1,-1,  -1,1,
        -1,1,   1,-1,   1,1
    ]), gl.STATIC_DRAW);

    const loc = gl.getAttribLocation(prog, "pos");
    gl.enableVertexAttribArray(loc);
    gl.vertexAttribPointer(loc, 2, gl.FLOAT, false, 0, 0);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    
    gl.viewport(0,0,canvas.width,canvas.height);
    
    const resLocation = gl.getUniformLocation(prog,"res");
    const hueLocation = gl.getUniformLocation(prog,"hue");
    const thinknessHUELocation = gl.getUniformLocation(prog,"thinknessHUE");
    const saturationLocation = gl.getUniformLocation(prog,"saturation");
    const lightnessLocation = gl.getUniformLocation(prog,"lightness");
    const hueMarkerPositionLocation = gl.getUniformLocation(prog,"hueMarkerPosition");
    const colorMarkerPositionLocation = gl.getUniformLocation(prog,"colorMarkerPosition");
    const rectLocation = gl.getUniformLocation(prog,"rec");

    function draw(obj){
        let hueMarkerPosition = obj.hueMarkerPosition();
        let colorMarkerPosition = obj.colorMarkerPosition();
        let color = obj.color();
        
        gl.uniform1f(thinknessHUELocation, obj.thinknessMarker);
        gl.uniform2f(rectLocation, obj.rect, obj.rect);
        gl.uniform1f(hueLocation, color.hsl.h/255.0);
        gl.uniform2f(resLocation, canvas.width,canvas.height);
        gl.uniform2f(hueMarkerPositionLocation, hueMarkerPosition.x, hueMarkerPosition.y);
        gl.uniform2f(colorMarkerPositionLocation, colorMarkerPosition.x, colorMarkerPosition.y);

        gl.drawArrays(gl.TRIANGLES, 0, 6);
    }
    return {
        draw
    };
}
export async function Chromatic(options){
    const RAD_TO_DEG = 180 / Math.PI;
    const DEG_TO_RAD = Math.PI / 180;

    const canvas = document.getElementById("color-picker");

    let _colorFactory = ColorFactory();
    let _color = _colorFactory.buildByHSL(0, 100, 50);

    const render = await ChromaticRender(canvas);

    const width = canvas.width;
    const height = canvas.height;

    const offset = 5;
    const cx = width / 2;
    const cy = height / 2;
    const outerRadius = Math.min(height, width)*0.5;
    const thinknessMarker = outerRadius*0.1;
    const innerRadius = outerRadius - thinknessMarker;
    const centerRadius = innerRadius + thinknessMarker*0.5;
    const rect = (innerRadius-offset-offset)* Math.sqrt(2);

    let picker = computeHSLPicker(rect, {x: cx,y:cy});

    let opt = {
        rect,
        thinknessMarker,
        hueMarkerPosition: ()=>{
            const rad = _color.hsl.h * DEG_TO_RAD;
            
            return {
                x: Math.cos(rad) * centerRadius + cx,
                y: Math.sin(rad) * centerRadius + cy
            };
        },
        colorMarkerPosition: ()=> {
            return picker.getPositionColor(_color.hsl.s, _color.hsl.l);
        },
        color: ()=>{ return _color; }
    };

    render.draw(opt);

    let touchID;
    canvas.addEventListener("mousedown", (e)=>{ 
        e.preventDefault();
        let rect = canvas.getBoundingClientRect();
        let cursorX = e.clientX - rect.left;
        let cursorY = e.clientY - rect.top;

        eventPressPicker(cursorX, cursorY);
    });
    canvas.addEventListener("touchstart", (e)=>{ 
        e.preventDefault();

        if(e.touches.length > 1)
            return;

        touchID = e.changedTouches[0].identifier;
        let rect = canvas.getBoundingClientRect();
        let cursorX = e.touches[0].clientX - rect.left;
        let cursorY = e.touches[0].clientY - rect.top;

        eventPressPicker(cursorX, cursorY);
    });
    function eventPressPicker(cursorX, cursorY){
        if(picker.isInside({x:cursorX, y:cursorY}, offset))
        {
            onUpdateColor(cursorX, cursorY);
            return;
        }

        let dx = cursorX - cx;
        let dy = cursorY - cy;
        let dist2 = dx*dx + dy*dy;

        if(!(dist2 < (innerRadius-offset)*(innerRadius-offset) || dist2 > (outerRadius+offset)*(outerRadius+offset)))
        {
            onUpdateHue(cursorX, cursorY);
            return;
        }
    }
    function onUpdateColor(cursorX, cursorY){
        let abort = new AbortController();
        
        setColorByPoint(cursorX, cursorY);

        window.addEventListener("mousemove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.clientX - rect.left;
            let cursorY = e.clientY - rect.top;

            setColorByPoint(cursorX, cursorY);
        }, { signal: abort.signal });

        window.addEventListener("touchmove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.touches[0].clientX - rect.left;
            let cursorY = e.touches[0].clientY - rect.top;
            setColorByPoint(cursorX, cursorY);
        }, { signal: abort.signal });

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("touchend", (e)=>{
            for(let i = 0; i < e.changedTouches.length; i++){
                if(e.changedTouches[i].identifier == touchID){
                    abort.abort()
                }
            }
        }, {once:true});
        window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
    }
    function onUpdateHue(cursorX, cursorY){
        let abort = new AbortController();
        setHueByPosition(cursorX, cursorY);
        
        window.addEventListener("mousemove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.clientX - rect.left;
            let cursorY = e.clientY - rect.top;

            setHueByPosition(cursorX, cursorY);
            options?.onUpdateColor(_color);
        }, { signal: abort.signal });

        window.addEventListener("touchmove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.touches[0].clientX - rect.left;
            let cursorY = e.touches[0].clientY - rect.top;

            setHueByPosition(cursorX, cursorY);
            options?.onUpdateColor(_color);
        }, { signal: abort.signal });
        

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("touchend", (e)=>{
            for(let i = 0; i < e.changedTouches.length; i++){
                if(e.changedTouches[i].identifier == touchID){
                    abort.abort()
                }
            }
        }, {once:true});

        window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
    }
    function setHueByPosition(x, y){
        const rad = Math.atan2(y - cy, x - cx);
        const degree = (rad * RAD_TO_DEG + 360) % 360;

        _color = _colorFactory.buildByHSL(degree, _color.hsl.s,_color.hsl.l);

        requestAnimationFrame(()=>render.draw(opt));
    }
    function setColorByPoint(x, y){
        const colorMarkerPosition = picker.clampped(x, y);
        _color = picker.getColor(_color.hsl.h, colorMarkerPosition);
        
        requestAnimationFrame(()=>render.draw(opt));
        options?.onUpdateColor(_color);
    }
    function setColor(color){
        _color = color;
        requestAnimationFrame(()=>render.draw(opt));
        options?.onUpdateColor(_color);
    }

    return {
        setColor
    }
}

function computeHSLPicker(rect, center){
    const _colorFactory = ColorFactory();
    const bounding = {
        min: {
            x:  Math.floor(center.x - rect*0.5),
            y:  Math.floor(center.y - rect*0.5)
        },
        max:{
            x:  Math.floor(center.x+ rect*0.5),
            y:  Math.floor(center.y+ rect*0.5)
        },
        width: function(){ return this.max.x - this.min.x;},
        height: function(){ return this.max.y - this.min.y;}
    };

    function getPositionColor(saturation, lightness) {            
        const x = saturation/100*bounding.width() + bounding.min.x;
        const y = (1-lightness/100)*bounding.height() + bounding.min.y;

        return clampped(x, y);
    }
    function getColor(hue, point){
        const u = (point.x - bounding.min.x) / (bounding.width());
        const v = (point.y - bounding.min.y) / (bounding.height());

        const h = hue;
        const s = Math.round(u * 100);
        const l = Math.round((1 - v) * 100);

        return _colorFactory.buildByHSL(h,s,l);
    }
    function isInside({x, y}, offset){
        return x >= bounding.min.x-offset && x <= bounding.max.x + offset
            && y >= bounding.min.y-offset && y <= bounding.max.y + offset;
    }
    function clampped(x, y) {
        return {
            x: Math.max(bounding.min.x, Math.min(bounding.max.x, x)),
            y: Math.max(bounding.min.y, Math.min(bounding.max.y, y))
        }
    }
    return {
        getColor,
        isInside,
        getPositionColor,
        clampped,
    }
}
export function ColorFactory(){
    function buildByDecimal(bigint, littlendian = true){
        let r, g, b, a;
        if(littlendian){
            r = (bigint >>> 24) & 0xFF;
            g = (bigint >>> 16) & 0xFF;
            b = (bigint >>> 8) & 0xFF;
            a = bigint & 0xFF;
        } else{
            r = bigint & 0xFF;
            g = (bigint >>> 8) & 0xFF;
            b = (bigint >>> 16) & 0xFF;
            a = (bigint >>> 24) & 0xFF;
        }
        
        return {
            rgb: {r, g, b},
            hsl: rgbToHsl(r, g, b),
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            littleEndian: getRGBLittleEndian(r,g,b)
        }
    }
    function buildByRGB(r,g,b){
        return {
            rgb: {r:parseInt(r), g:parseInt(g), b:parseInt(b)},
            hsl: rgbToHsl(r, g, b),
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            littleEndian: getRGBLittleEndian(r,g,b)
        }
    }
    function buildByHSL(h,s,l){
        const {r,g,b} = hslToRgb(h,s,l);
        return {
            rgb: {r,g,b},
            hsl: { h, s, l},
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            littleEndian: getRGBLittleEndian(r,g,b)
        };
    }
    function buildByHex(hex){
        const {r,g,b} = hexToRgb(hex);

        return {
            rgb: {r,g,b},
            hsl: rgbToHsl(r, g, b),
            hex: hex,
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            littleEndian: getRGBLittleEndian(r,g,b)
        }
    }
    
    function rgbToHex(r, g, b) {
        return  "#" + rgbToHex16(r, g, b).toString(16).padStart(6, "0");
    }
    function rgbToHex16(r, g, b) {
        return  (r << 16 | g << 8 | b);
    }
    function rgbToHex32(r, g, b) {
        return ((r << 24) | (g << 16) | (b << 8) | (0xFF)) >>> 0;
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
        hex = hex.replace(/^#/, "").slice(0, 6);
        if(hex == 0){
            return {r: 0, g: 0, b: 0};
        }
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
    function getRGBLittleEndian(r,g,b){
        return ((0xFF << 24) | (b << 16) | (g << 8) | (r)) >>> 0;
    }
    return {
        buildByDecimal,
        buildByRGB,
        buildByHSL,
        buildByHex
    }
}