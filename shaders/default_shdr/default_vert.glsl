# version 330 core

layout (location=0) in vec3 position;
layout (location=1) in vec4 color;

out vec4 fragColor;

void main(){

	gl_Position = vec4(position, 1.0);
	fragColor = color;
}