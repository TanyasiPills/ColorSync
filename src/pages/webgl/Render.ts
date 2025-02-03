import { IndexBuffer } from "./IndexBuffer";
import { VertexArray } from "./Shaders/VertexArray ";

interface RenderData {
    va: VertexArray;
    ib: IndexBuffer;
    shader: NewShader;//need these files first oops
    texture: MyTexture;
}

const renderData: RenderData = {
    va: new VertexArray(),
    ib: new IndexBuffer(),
    shader: new NewShader(),
    texture: new MyTexture(),
};

export class Render {
    private gl: WebGL2RenderingContext;
}