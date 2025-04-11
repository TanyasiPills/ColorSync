export const vertexShaderSource = `
    #version 300 es
    precision mediump float;

    layout(location = 0) in vec4 position;
    layout(location = 1) in vec2 texCoord;

    out vec2 v_TexCoord;

    void main() {
        gl_Position = position;
        v_TexCoord = texCoord;
    }
`;

export const fragmentShaderSource = `
    #version 300 es
    precision mediump float;

    in vec2 v_TexCoord;
    layout(location = 0) out vec4 color;

    uniform vec4 u_Color;
    uniform sampler2D u_Texture;
    uniform float opacity;

    void main() {
        vec4 texColor = texture(u_Texture, v_TexCoord);

        if (texColor.a * opacity < 0.01) discard;

        texColor.a *= opacity;
        texColor.rgb *= texColor.a;
        color = texColor;
    }
`;
