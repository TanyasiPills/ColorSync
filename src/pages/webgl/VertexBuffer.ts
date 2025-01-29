export class VertexBuffer {
    private buffer: WebGLBuffer | null;
    private gl: WebGL2RenderingContext;

    constructor(gl: WebGL2RenderingContext, data: ArrayBuffer) {
        this.gl = gl;
        this.buffer = gl.createBuffer();
        if (!this.buffer) {
            throw new Error("Failed to create buffer");
        }

        gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);
        gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
    }

    bind() {
        if (this.buffer) {
            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer);
        }
    }

    unbind() {
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, null);
    }

    delete() {
        if (this.buffer) {
            this.gl.deleteBuffer(this.buffer);
            this.buffer = null;
        }
    }
}