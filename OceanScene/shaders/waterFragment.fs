#version 330 core

in vec2 textureCoords;

out vec4 out_Color;


uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;


void main(void) {

	vec4 reflectColour = texture(reflectionTexture, textureCoords);
	vec4 refractColour = texture(refractionTexture, textureCoords);

	out_Color = mix(reflectColour, refractColour, 0.5);

}
