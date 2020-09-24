#version 330 core

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCameraVector;
out vec4 out_Color;


uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform float time;

const float waveStrength = .02;
const float waveSpeed = 0.03;


void main(void) {

	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;

	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, 1.0-ndc.y);

	vec2 distortion1 = texture(dudvMap, vec2(textureCoords.x + waveSpeed * time,
		                                       textureCoords.y)).rg * 2.0 - 1.0;

  vec2 distortion2 = texture(dudvMap, vec2(-textureCoords.x,
		                                         textureCoords.y + waveSpeed * time)).rg * 2.0 - 1.0;

	vec2 distortion = distortion1 + distortion2;

	reflectTexCoords += distortion * waveStrength;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	refractTexCoords += distortion * waveStrength;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);

	// frenel effects
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0., 1., 0.));


	//out_Color = reflectColour;
	out_Color = mix(reflectColour, refractColour, refractiveFactor);

}
