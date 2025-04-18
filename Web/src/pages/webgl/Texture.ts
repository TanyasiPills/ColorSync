export class Texture {
    private gl: WebGL2RenderingContext;
    private texture: WebGLTexture;
    private width: number = 0;
    private height: number = 0;

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.texture = gl.createTexture()!;
    }

    public init(path: string): void;
    public init(width: number, height: number, transparent: number): void;
    public init(data: Uint8Array , width: number, height: number): void;

    public init(arg1: string | Uint8Array  | number, arg2?: number, arg3?: number): void {
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);
        
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.CLAMP_TO_EDGE);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.CLAMP_TO_EDGE);

        if (typeof arg1 === "string") {
            const image = new Image();
            image.src = arg1;
            image.onload = () => {
                this.width = image.width;
                this.height = image.height;

                this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);

                this.gl.texImage2D(
                    this.gl.TEXTURE_2D,
                    0,
                    this.gl.RGBA,
                    this.gl.RGBA,
                    this.gl.UNSIGNED_BYTE,
                    image
                );

                this.gl.bindTexture(this.gl.TEXTURE_2D, null);
            };
            image.onerror = () => {
                console.error("Failed to load texture:", arg1);
            };
        } else if (typeof arg1 === "number") {
            this.width = arg2!;
            this.height = arg3!;

            const data = new Uint8Array(this.width * this.height * 4);
            for (let i = 0; i < data.length; i++) {
                data[i] = (i + 1) % 4 === 0 ? ((arg3) ? 0 : 255) : 100;
            }

            this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);

            this.gl.texImage2D(
                this.gl.TEXTURE_2D,
                0,
                this.gl.RGBA,
                this.width,
                this.height,
                0,
                this.gl.RGBA,
                this.gl.UNSIGNED_BYTE,
                data
            );

            this.gl.bindTexture(this.gl.TEXTURE_2D, null);
        } else {
            this.width = arg2!;
            this.height = arg3!;

            this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);

            this.gl.texImage2D(
                this.gl.TEXTURE_2D,
                0,
                this.gl.RGBA,
                this.width,
                this.height,
                0,
                this.gl.RGBA,
                this.gl.UNSIGNED_BYTE,
                arg1
            );

            this.gl.bindTexture(this.gl.TEXTURE_2D, null);

            const texture = this.gl.createTexture();
            this.gl.bindTexture(this.gl.TEXTURE_2D, texture);

            this.gl.pixelStorei(this.gl.UNPACK_ALIGNMENT, 1);

            this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
            this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
            this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.CLAMP_TO_EDGE);
            this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.CLAMP_TO_EDGE);

            this.gl.bindTexture(this.gl.TEXTURE_2D, null);
        }

        this.gl.bindTexture(this.gl.TEXTURE_2D, null);
    }

    bind(slot: number = 0): void {
        this.gl.activeTexture(this.gl.TEXTURE0 + slot);
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);
    }
    
    unbind() {
        this.gl.bindTexture(this.gl.TEXTURE_2D, null);
    }

    getId(): WebGLTexture {
        return this.texture;
    }
}
