export class VertexBufferElement {
    type: number;
    count: number;
    normalized: boolean;

    constructor(type: number, count: number, normalized: boolean) {
        this.type = type;
        this.count = count;
        this.normalized = normalized;
    }

    static getSizeOfType(type: number): number {
        switch (type) {
            case WebGL2RenderingContext.FLOAT: return 4;
            case WebGL2RenderingContext.UNSIGNED_INT: return 4;
            case WebGL2RenderingContext.UNSIGNED_BYTE: return 1;
            default: return 0;
        }
    }
}

export class VertexBufferLayout {
    private elements: VertexBufferElement[] = [];
    private stride: number = 0;

    pushFloat(count: number): void {
        this.elements.push({ type: WebGLRenderingContext.FLOAT, count, normalized: false });
        this.stride += count * this.getSizeOfType(WebGLRenderingContext.FLOAT);
    }

    pushUnsignedInt(count: number): void {
        this.elements.push({ type: WebGLRenderingContext.UNSIGNED_INT, count, normalized: false });
        this.stride += count * this.getSizeOfType(WebGLRenderingContext.UNSIGNED_INT);
    }

    pushUnsignedByte(count: number): void {
        this.elements.push({ type: WebGLRenderingContext.UNSIGNED_BYTE, count, normalized: true });
        this.stride += count * this.getSizeOfType(WebGLRenderingContext.UNSIGNED_BYTE);
    }

    getElements(): VertexBufferElement[] {
        return this.elements;
    }

    getStride(): number {
        return this.stride;
    }

    private getSizeOfType(type: number): number {
        switch (type) {
            case WebGLRenderingContext.FLOAT: return 4;
            case WebGLRenderingContext.UNSIGNED_INT: return 4;
            case WebGLRenderingContext.UNSIGNED_BYTE: return 1;
            default: throw new Error(`Unknown type: ${type}`);
        }
    }
}
