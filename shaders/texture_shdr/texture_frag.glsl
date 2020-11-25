# version 330 core

in vec4 fragColor;
in vec2 textureUV;
// this shader assumes one texture so no texture index required

out vec4 outColor;

uniform sampler2D myTex;

void main(){

	outColor = texture(myTex, textureUV) * fragColor;
}