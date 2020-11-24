# version 330 core

in vec3 fragColor;
in vec2 textureUV;

out vec4 outColor;

uniform sampler2D myTex;

void main(){

	outColor = texture(myTex, textureUV) * vec4(fragColor, 1.0);
}