# version 330 core

layout (location=0) in vec3 position;

out vec4 fragColor;

uniform vec4 userColor;

void main(){

	gl_Position = vec4(position, 1.0);
	fragColor = userColor;
}