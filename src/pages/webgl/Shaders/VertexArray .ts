import { VertexBuffer } from "../VertexBuffer";
import { VertexBufferLayout, VertexBufferElement } from "../VertxBufferLayout";

export class VertexArray {
    private vao: WebGLVertexArrayObject | null;
    private gl: WebGL2RenderingContext;
    private layout: VertexBufferLayout;

    constructor(gl: WebGL2RenderingContext, layout: VertexBufferLayout) {
        this.gl = gl;
        this.vao = gl.createVertexArray();
        this. layout = layout;
        if (!this.vao) {
            throw new Error("Failed to create VAO");
        }
    }
    setLayout(layoutIn: VertexBufferLayout){
        this.layout = layoutIn;
    }
    setBuffer(vertexBuffer:VertexBuffer){
        this.bind();
        vertexBuffer.bind()
        let elements = this.layout.getElements();
        let offset: number = 0;
        for (let i = 0; i < elements.length; i++) {
            let element = elements[i]
            this.gl.enableVertexAttribArray(i);
            this.gl.vertexAttribPointer(i, element.count, element.type, element.normalized, this.layout.getStride(), offset);
            offset += element.count * VertexBufferElement.getSizeOfType(element.type);

        }
    }
    bind() {
        if (this.vao) {
            this.gl.bindVertexArray(this.vao);
            
        }
    }

    unbind() {
        this.gl.bindVertexArray(null);
    }

    delete() {
        if (this.vao) {
            this.gl.deleteVertexArray(this.vao);
            this.vao = null;
        }
    }
}