# version 330 core
// this shader can handle 4 textures
in vec4 fragColor;
in vec2 textureUV;
in float textureIndex;

out vec4 outColor;

uniform sampler2D myTex[4];

void main(){

	// get the proper texture from the array
	int index = int(textureIndex);
	outColor = texture(myTex[index], textureUV) * fragColor;
}