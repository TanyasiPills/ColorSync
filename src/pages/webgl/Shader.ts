export class Shader {
    private gl: WebGL2RenderingContext;
    private program: WebGLProgram;
    private uniformLocations: Map<string, WebGLUniformLocation | null>;

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.program = gl.createProgram()!;
        this.uniformLocations = new Map();
    }

    public async bindShaderFile(filepath: string): Promise<void> {
        const source = await this.parseShader(filepath);
        this.program = this.createShader(source.vertex, source.fragment);
    }

    public bind(): void {
        this.gl.useProgram(this.program);
    }

    public unbind(): void {
        this.gl.useProgram(null);
    }

    private async parseShader(filepath: string): Promise<{ vertex: string; fragment: string }> {
        const response = await fetch(filepath);
        const source = await response.text();

        let vertex = "";
        let fragment = "";
        let type: "VERTEX" | "FRAGMENT" | null = null;

        source.split("\n").forEach((line) => {
            if (line.includes("#shader vertex")) {
                type = "VERTEX";
            } else if (line.includes("#shader fragment")) {
                type = "FRAGMENT";
            } else if (type === "VERTEX") {
                vertex += line + "\n";
            } else if (type === "FRAGMENT") {
                fragment += line + "\n";
            }
        });

        return { vertex, fragment };
    }

    private createShader(vertexSource: string, fragmentSource: string): WebGLProgram {
        const program = this.gl.createProgram()!;
        const vertShader = this.compileShader(this.gl.VERTEX_SHADER, vertexSource);
        const fragShader = this.compileShader(this.gl.FRAGMENT_SHADER, fragmentSource);

        this.gl.attachShader(program, vertShader);
        this.gl.attachShader(program, fragShader);
        this.gl.linkProgram(program);

        if (!this.gl.getProgramParameter(program, this.gl.LINK_STATUS)) {
            console.error("Shader program failed to link:", this.gl.getProgramInfoLog(program));
            this.gl.deleteProgram(program);
            throw new Error("Shader link failed");
        }

        this.gl.deleteShader(vertShader);
        this.gl.deleteShader(fragShader);

        return program;
    }

    private compileShader(type: number, source: string): WebGLShader {
        const shader = this.gl.createShader(type);
        if (!shader) throw new Error("Failed to create shader");

        this.gl.shaderSource(shader, source);
        this.gl.compileShader(shader);

        if (!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
            console.error("Shader compilation failed:", this.gl.getShaderInfoLog(shader));
            this.gl.deleteShader(shader);
            throw new Error("Shader compilation failed");
        }

        return shader;
    }

    public getUniformLocation(name: string): WebGLUniformLocation | null {
        if (this.uniformLocations.has(name)) {
            return this.uniformLocations.get(name)!;
        }
        const location = this.gl.getUniformLocation(this.program, name);
        this.uniformLocations.set(name, location);
        return location;
    }

    public setUniform1i(name: string, i1: number): void {
        this.gl.uniform1i(this.getUniformLocation(name), i1);
    }

    public setUniform1f(name: string, f1: number): void {
        this.gl.uniform1f(this.getUniformLocation(name), f1);
    }

    public setUniform3f(name: string, f1: number, f2: number, f3: number): void {
        this.gl.uniform3f(this.getUniformLocation(name), f1, f2, f3);
    }

    public setUniform4f(name: string, f1: number, f2: number, f3: number, f4: number): void {
        this.gl.uniform4f(this.getUniformLocation(name), f1, f2, f3, f4);
    }

    public setUniform4Matrix(name: string, value: Float32Array): void {
        this.gl.uniformMatrix4fv(this.getUniformLocation(name), false, value);
    }
}
