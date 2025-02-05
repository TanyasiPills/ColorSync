import { CanvasData, Drawing } from "./Drawing";
import { IndexBuffer } from "./IndexBuffer";
import { Shader } from "./Shader";
import { VertexArray } from "./Shaders/VertexArray ";
import { Texture } from "./Texture";
import { VertexBufferLayout } from "./VertxBufferLayout";

export class RenderData {
    va!: VertexArray;
    ib!: IndexBuffer;
    shader!: Shader;
    texture!: Texture;
}


export class Render {
    private gl: WebGL2RenderingContext;
    private layers: RenderData[];
    public cursor!: RenderData;

    private cursorRadius: number;
    private initialCanvasRatio: [number, number];
    private canvasRatio: Float32Array;
    private identityRatio: [number, number];
    private offset: Float32Array;
    private cursorScale: [number, number, number];
    private identityOffset: [number, number];
    private prevPos: [number, number];
    private canvasSize: [number, number];
    private fbo: WebGLFramebuffer | null;
    private drawing: Drawing | null;

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.layers = [];
        this.cursor = new RenderData();
        this.drawing = null;
    
        this.cursorRadius = 0.01;
        this.initialCanvasRatio = [1.0, 1.0];
        this.canvasRatio = new Float32Array([1.0, 1.0]);
        this.identityRatio = [1.0, 1.0];
        this.offset = new Float32Array([0, 0]);
        this.cursorScale = [1.0, 1.0, 1.0];
        this.identityOffset = [0, 0];
        this.prevPos = [0, 0];
        this.canvasSize = [1, 1];
        this.fbo = null;
    }
    
    init(canvasWIn: number, canvasHIn: number, screenW: number, screenH: number): void {
        this.fbo = this.gl.createFramebuffer();
        this.canvasSize[0] = canvasWIn;
        this.canvasSize[1] = canvasHIn;
        this.drawing = new Drawing(this.gl, canvasWIn, canvasHIn);
        this.drawing.initBrush(this.cursor, this.cursorRadius);
        const canvasData: CanvasData = this.drawing.initCanvas(canvasWIn, canvasHIn);
        this.layers.push(canvasData.data);
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbo);
        this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, this.gl.COLOR_ATTACHMENT0, this.gl.TEXTURE_2D, canvasData.data.texture.getId(), 0);
        if (this.gl.checkFramebufferStatus(this.gl.FRAMEBUFFER) != this.gl.FRAMEBUFFER_COMPLETE) {
            console.error(this.gl.checkFramebufferStatus(this.gl.FRAMEBUFFER));
        }
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, 0);
        this.initialCanvasRatio[0] = canvasData.canvasX;
        this.initialCanvasRatio[1] = canvasData.canvasY;
    }

    moveLayers(offsetIn: Float32Array): void{
        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];
        for (let item of this.layers){
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset);
        }
    }


    zoom(scale: number, offsetIn: Float32Array): void{
        this.offset[0] *= scale;
        this.offset[1] *= scale;
        this.initialCanvasRatio[0] *= scale;
        this.initialCanvasRatio[1] *= scale;
        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];

        for (let item of this.layers) {
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset)
        }

        this.cursorRadius *= scale;
    }

    onResize(x: number, y: number, offsetIn: Float32Array, yRatio: number): void {
        this.cursorScale[0] = x;
        this.cursorScale[1] = y * yRatio;
        this.canvasRatio[0] = x * this.initialCanvasRatio[0];
        this.canvasRatio[1] = y * this.initialCanvasRatio[1];

        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];

        
        for (let item of this.layers) {
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset)
        }
    }

    loadPrevCursor(prevIn: Float32Array): void {
        this.prevPos[0] = prevIn[0];
        this.prevPos[1] = prevIn[1];
    }

    renderCursorToCanvas(): void {
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbo);
        this.gl.viewport(0, 0, this.canvasSize[0], this.canvasSize[1]);
        //var pos: Float32Array = 
        
    }


}