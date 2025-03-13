#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;


void main()
{
	gl_Position = position;
	v_TexCoord = texCoord;
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

uniform vec3 Kolor;

void main() {
	vec4 texColor = texture(u_Texture, v_TexCoord);
	if (texColor.a < 0.01) discard;
	color = texColor * vec4(Kolor, texColor.a);
}