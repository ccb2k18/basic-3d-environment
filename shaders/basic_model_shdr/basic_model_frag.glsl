# version 330 core

// light direction vector
uniform vec3 lightDir;

in vec4 fragColor;
//in vec3 surfaceNormal;

out vec4 outColor;

void main(){

	// normalize between 0 and 1
	//float scalar = 0.5*dot(lightDir, surfaceNormal) + 0.5;
	outColor = fragColor;// * scalar;
}