import { RenderData } from "./Render";
import { VertexBuffer } from "./VertexBuffer";
import { VertexBufferLayout } from "./VertxBufferLayout";

export class CanvasData{
    public data!: RenderData;
    public canvasX!: number;
    public canvasY!: number;
    constructor(){
        this.data = new RenderData();
    }
}

export class Drawing{
    private gl: WebGL2RenderingContext;
    private canvasW: number;
    private canvasH: number;

    constructor(gl: WebGL2RenderingContext, canvasW: number, canvasH: number){
        this.gl = gl;
        this.canvasW = canvasW;
        this.canvasH = canvasH;
    }

    fillPositions(positions: Float32Array, xScale: number = 0.0, yScale: number = 0.0, xPos: number = 0.0, yPos: number = 0.0): void {
        positions[0] = (-xScale) + xPos; positions[1] = (-yScale) + yPos; positions[2] = 0.0; positions[3] = 0.0;
        positions[4] = xScale + xPos; positions[5] = (-yScale) + yPos; positions[6] = 1.0; positions[7] = 0.0;
        positions[8] = xScale + xPos; positions[9] = yScale + yPos; positions[10] = 1.0; positions[11] = 1.0;
        positions[12] = (-xScale) + xPos; positions[13] = yScale + yPos; positions[14] = 0.0; positions[15] = 1.0;
    }

    initData(data: RenderData, positions: Float32Array, texture: string | null, shaderType: number = 0, transparent:number = 0): void{
        const indices: Uint32Array = new Uint32Array([0, 1, 2, 2, 3, 0]);
        const vb: VertexBuffer = new VertexBuffer(this.gl, positions)
        var layout: VertexBufferLayout = new VertexBufferLayout();
        layout.pushFloat(2);
        layout.pushFloat(2);
        data.va.setLayout(layout);
        data.va.setBuffer(vb);
        data.ib.init(indices);

        var shader: string;
        if (shaderType == 0) {
            shader = "Resources/Shaders/LayerShader.ts";
        }
        else {
            shader = "Resources/Shaders/CursorShader.ts";
        }
        data.shader.bindShaderFile(shader);
        data.shader.bind();
        if (texture != null) {
            data.texture.init(texture);
        }
        else {
            data.texture.init(this.canvasW, this.canvasH, transparent)
        }
        data.texture.bind();
        data.shader.setUniform1i("u_Texture", 0);

        data.va.unbind();
        data.ib.unbind();
        data.shader.unbind();
        data.texture.unbind();
    }

    initCanvas(canvasWIn: number, canvasHIn: number){
        var data: CanvasData = new CanvasData();
        this.canvasW = canvasWIn;
        this.canvasH = canvasHIn;
        var canvasRatio: number;
        const positions: Float32Array = new Float32Array(16);
        var xScale;
        var yScale;
        if (canvasWIn >= canvasHIn) {
            canvasRatio = canvasHIn / canvasWIn;
            xScale = 0.8;
            yScale = canvasRatio * 0.8;
        }
        else {
            canvasRatio = canvasWIn / canvasHIn;
            xScale = 0.8;
            yScale = canvasRatio * 0.8;
        }
        this.fillPositions(positions, xScale, yScale);
        this.initData(data.data, positions, null);

        data.data.fbo = this.gl.createBuffer();
        this.gl.bindBuffer(this.gl.FRAMEBUFFER, data.data.fbo);
        this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, this.gl.COLOR_ATTACHMENT0, this.gl.TEXTURE_2D, data.data.texture.getId(), 0);
        this.gl.bindBuffer(this.gl.FRAMEBUFFER, 0);

        data.canvasX = xScale;
        data.canvasY = yScale;

        return data;
    }

    initLayer(data: RenderData, xScale: number, yScale: number): void{
        const positions: Float32Array = new Float32Array(16);
        this.fillPositions(positions, xScale, yScale);
        this.initData(data, positions, null);
    }

    public initBrush(data: RenderData, radius: number, texture: string|null): void{
        const positions: Float32Array = new Float32Array(16);
        this.fillPositions(positions, radius, radius);
        if (texture == null) {
            this.initData(data, positions, "Shaders/Textures/penBrush.png", 1);
        } else{
            this.initData(data, positions, texture.toString(), 1);
        }
        
    }

    brushToPosition(cursor: RenderData, radius: number, aspect: [number, number], offset: [number, number], scale: [number, number, number], position: Float32Array): void {
        const positions: Float32Array = new Float32Array(16);
        const yMult: number = aspect[0] / aspect[1];
        this.fillPositions(positions, radius/aspect[0], radius * yMult / aspect[0] * scale[1], (position[0] - offset[0]) / aspect[0], (position[1] - offset[1]) / aspect[1]);
        const vb: VertexBuffer = new VertexBuffer(this.gl, positions);
        cursor.va.setBuffer(vb);
        cursor.va.unbind();
    }

    public moveCanvas(canvas: RenderData, size: Float32Array, offset: [number, number]): void{
        const positions: Float32Array = new Float32Array(16);
        this.fillPositions(positions, size[0], size[1], offset[0], offset[1]);
        const vb: VertexBuffer = new VertexBuffer(this.gl, positions);
        canvas.va.setBuffer(vb);
        canvas.va.unbind();
    }
}