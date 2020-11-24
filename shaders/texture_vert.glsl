# version 330 core

layout (location=0) in vec3 pos;
layout (location=1) in vec3 clr;
layout (location=2) in vec2 texUV;

out vec3 fragColor;
out vec2 textureUV;

void main(){

	gl_Position = vec4(pos, 1.0);
	fragColor = clr;
	textureUV = texUV;
}