export class IndexBuffer {
    private gl: WebGL2RenderingContext;
    private ibo: WebGLBuffer | null;
    private count: number;

    constructor(gl: WebGL2RenderingContext, data: Uint32Array, count: number) {
        this.gl = gl;
        this.ibo = gl.createBuffer();
        this.count = count;

        if (!this.ibo) {
            throw new Error("Failed to create Index Buffer Object (IBO)");
        }

        this.init(data);
    }

    private init(data: Uint32Array): void {
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, this.ibo);
        this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, data, this.gl.DYNAMIC_DRAW);
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, null);
    }

    public bind(): void {
        if (this.ibo) {
            this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, this.ibo);
        }
    }

    public unbind(): void {
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, null);
    }

    public delete(): void {
        if (this.ibo) {
            this.gl.deleteBuffer(this.ibo);
            this.ibo = null;
        }
    }

    public getCount(): number {
        return this.count;
    }
}
