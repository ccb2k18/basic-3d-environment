# version 330 core

// light direction vector
uniform vec3 lightDir;

in vec4 fragColor;
in vec3 surfaceNormal;

out vec4 outColor;

void main(){

	// normalize between 0 and 1
	float scalar = max(0.5*dot(lightDir, surfaceNormal) + 0.5, 0.25);
	//float scalar = max(dot(lightDir, surfaceNormal), 1.0);
	outColor = vec4(fragColor.xyz, 1.0) * scalar;
}