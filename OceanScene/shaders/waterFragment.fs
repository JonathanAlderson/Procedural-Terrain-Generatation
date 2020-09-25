#version 330 core

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCameraVector;
out vec4 out_Color;


uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform float time;

const float waveSpeed = 0.0;
const float noiseScale = .01;

const vec4 oceanColour = vec4(112./255., 214./255., 255./255., 1.);
const float oceanBlend = 0.2;
const float waveStrength = 0.02;


void main(void) {

	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;

	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + waveSpeed * time,
		                                       textureCoords.y )).rg * 2.0 - 1.0) * waveStrength;

  /* vec2 distortion2 = texture(dudvMap, vec2(-textureCoords.x * noiseScale,
		                                        textureCoords.y * noiseScale + waveSpeed * time)).rg; // * 2.0 - 1.0; */

	vec2 distortion = distortion1; // + distortion2;

	reflectTexCoords += distortion;
	refractTexCoords += distortion;

	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.01, 0.99);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.99, -0.01);

	refractTexCoords = clamp(refractTexCoords, 0.01, 0.99);

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);

	// frenel effects
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0., 1., 0.));


	// If above water to fresnel effect
	out_Color = mix(reflectColour, refractColour, refractiveFactor);


	// If underwater, only do refraction
	if(toCameraVector.y < 0)
	{
		out_Color = refractColour;
	}


	// Add ocean colour
	out_Color = mix(out_Color, oceanColour, oceanBlend);
}
