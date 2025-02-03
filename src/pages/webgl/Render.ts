import { IndexBuffer } from "./IndexBuffer";
import { Shader } from "./Shader";
import { VertexArray } from "./Shaders/VertexArray ";
import { Texture } from "./Texture";
import { VertexBufferLayout } from "./VertxBufferLayout";

interface RenderData {
    va: VertexArray;
    ib: IndexBuffer;
    shader: Shader;
    texture: Texture;
}



export class Render {
    private gl: WebGL2RenderingContext;
    private layers: RenderData[] = []; 
    private cursor: RenderData;

    private cursorRadius: number = 0.01;
    private initialCanvasRatio: [number, number] = [1.0, 1.0];
    private canvasRatio: [number, number] = [1.0, 1.0];
    private identityRatio: [number, number] = [1.0, 1.0];
    private offset: [number, number] = [0, 0];
    private cursorScale: [number, number, number] = [1.0, 1.0, 1.0];
    private identityOffset: [number, number] = [0, 0];
    private prevPos: [number, number] = [0, 0];
    private canvasSize: [number, number] = [1, 1];
    private fbo: WebGLFramebuffer | null = null;

    constructor(gl: WebGL2RenderingContext, layout: VertexBufferLayout, indexData: Uint32Array) {
        this.gl = gl;
        this.cursor = {
            va: new VertexArray(this.gl, layout),        
            ib: new IndexBuffer(this.gl, indexData, indexData.length), 
            shader: new Shader(this.gl),                 
            texture: new Texture(this.gl)                
        };
    }

}