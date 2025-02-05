import { RenderData } from "./Render";
import { VertexBuffer } from "./VertexBuffer";
import { VertexBufferLayout } from "./VertxBufferLayout";

class CanvasData{
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

    initData(data: RenderData, positions: Float32Array, texture: string | null, shaderType: number = 0): void{
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
            shader = "Resources/Shaders/CursorShader.ts";
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
            data.texture.init(this.canvasW, this.canvasH)
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
        var xScale = 1;
        var yScale = 1;
        if (canvasWIn >= canvasHIn) {
            canvasRatio = canvasHIn / canvasWIn;
        }
        else {
            canvasRatio = canvasWIn / canvasHIn;
        }
        this.fillPositions(positions, xScale, yScale);
        this.initData(data.data, positions, null);
        data.canvasX = xScale;
        data.canvasY = yScale;

        return data;
    }

    initLayer(data: RenderData, xScale: number, yScale: number): void{
        const positions: Float32Array = new Float32Array(16);
        this.fillPositions(positions, xScale, yScale);
        this.initData(data, positions, null);
    }

    initBrush(data: RenderData, radius: number): void{
        const positions: Float32Array = new Float32Array(16);
        this.fillPositions(positions, radius, radius);
        this.initData(data, positions, "Resources/Texture/circle.png", 1);
    }

    brushToPosition(cursor: RenderData, radius: number, aspect: Float32Array, offset: Float32Array, scale: Float32Array, position: Float32Array): void {
        const positions: Float32Array = new Float32Array(16);
        const yMult: number = aspect[0] / aspect[1];
        this.fillPositions(positions, radius/aspect[0], radius * yMult / aspect[0] * scale[1], (position[0] - offset[0]) / aspect[0], (position[1] - offset[1]) / aspect[1]);
        const vb: VertexBuffer = new VertexBuffer(this.gl, positions);
        cursor.va.setBuffer(vb);
        cursor.va.unbind();
    }

    moveCanvas(canvas: RenderData, size: Float32Array, offset: Float32Array): void{
        const positions: Float32Array = new Float32Array(16);
        this.fillPositions(positions, size[0], size[1], offset[0], offset[1]);
        const vb: VertexBuffer = new VertexBuffer(this.gl, positions);
        canvas.va.setBuffer(vb);
        canvas.va.unbind();
    }
}