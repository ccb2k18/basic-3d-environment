# version 330 core

layout (location=0) in vec3 position;
layout (location=1) in vec4 color;
layout (location=2) in vec3 normal;

// matrix uniforms for transforming the model

// model matrix

uniform mat4x4 translation;
uniform mat4x4 rotation;
uniform mat4x4 scale;

// view matrix

uniform mat4x4 cameraView;

// projection matrix

uniform mat4x4 perspective;

//uniform mat4x4 MVP;

uniform vec4 lightNormal;

out vec4 fragColor;
out vec3 surfaceNormal;

void main(){

	mat4x4 MVP = perspective * cameraView * translation * rotation * scale;
	gl_Position = MVP * vec4(position, 1.0);
	fragColor = color;
	surfaceNormal = normal;
}