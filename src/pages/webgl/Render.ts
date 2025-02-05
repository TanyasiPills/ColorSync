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
    private canvasRatio: [number, number];
    private identityRatio: [number, number];
    private offset: [number, number];
    private cursorScale: [number, number, number];
    private identityOffset: [number, number];
    private prevPos: [number, number];
    private canvasSize: [number, number];
    private fbo: WebGLFramebuffer | null;

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.layers = [];
        this.cursor = new RenderData();
    
        this.cursorRadius = 0.01;
        this.initialCanvasRatio = [1.0, 1.0];
        this.canvasRatio = [1.0, 1.0];
        this.identityRatio = [1.0, 1.0];
        this.offset = [0, 0];
        this.cursorScale = [1.0, 1.0, 1.0];
        this.identityOffset = [0, 0];
        this.prevPos = [0, 0];
        this.canvasSize = [1, 1];
        this.fbo = null;
    }
    
    init(): void {}



}