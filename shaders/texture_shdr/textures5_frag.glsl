# version 330 core
// this shader can handle 5 textures
in vec4 fragColor;
in vec2 textureUV;
in float textureIndex;

out vec4 outColor;

uniform sampler2D myTex[5];

void main(){

	// get the proper texture from the array
	int index = int(textureIndex);
	outColor = texture(myTex[index], textureUV) * fragColor;
}