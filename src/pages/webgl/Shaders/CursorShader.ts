export const vertexShaderSource = `#version 300 es
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main() {
    gl_Position = position;
    v_TexCoord = texCoord;
}
`;

export const fragmentShaderSource = `#version 300 es
precision mediump float;

in vec2 v_TexCoord;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

uniform vec3 Kolor;

void main() {
    vec4 textColor = texture(u_Texture, v_TextCoord)
    color = textColor * vec4(Kolor, 1.0);
}
`;
