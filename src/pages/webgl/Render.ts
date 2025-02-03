import { IndexBuffer } from "./IndexBuffer";
import { Shader } from "./Shader";
import { VertexArray } from "./Shaders/VertexArray ";

interface RenderData {
    va: VertexArray;
    ib: IndexBuffer;
    shader: Shader;
    texture: Texture;
}



export class Render {
    private gl: WebGL2RenderingContext;

    constructor(gl: WebGL2RenderingContext){
        this.gl = gl;
    }
    renderData: RenderData = {
        va: new VertexArray(this.gl),
        ib: new IndexBuffer(this.gl),
        shader: new Shader(this.gl),
        texture: new Texture(),
    };
}