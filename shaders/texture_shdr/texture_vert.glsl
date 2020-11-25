# version 330 core

layout (location=0) in vec3 pos;
layout (location=1) in vec4 clr;
layout (location=2) in vec2 texUV;
layout (location=3) in float texIndex;

out vec4 fragColor;
out vec2 textureUV;
out float textureIndex;

void main(){

	gl_Position = vec4(pos, 1.0);
	fragColor = clr;
	textureUV = texUV;
	textureIndex = texIndex;
}