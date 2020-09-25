#version 330 core

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCameraVector;
out vec4 out_Color;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float time;
uniform DirLight dirLight;

const float waveSpeed = 0.1;
const float noiseScale = .01;

const vec4 oceanColour = vec4(112./255., 214./255., 255./255., 1.);
const float oceanBlend = 0.2;
const float waveStrength = 0.02;

// for normals
const float shineDamper = 20.;
const float reflectivity = 0.6;


void main(void) {

	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;

	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 dist = (texture(dudvMap, vec2(textureCoords.x + waveSpeed * time),
	                                    textureCoords.y)).rg * 0.1;
	dist = textureCoords + vec2(dist.x, dist.y + waveSpeed * time);

	vec2 totalDist = (texture(dudvMap, dist).rg * 2.0 - 1.0) * waveStrength;

	reflectTexCoords += totalDist;
	refractTexCoords += totalDist;

	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.01, 0.99);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.99, -0.01);

	refractTexCoords = clamp(refractTexCoords, 0.01, 0.99);

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);

	// frenel effects
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0., 1., 0.));

	vec4 normMap = texture(normalMap, totalDist);
	vec3 normal = vec3(normMap.r * 2.0 - 1.0, normMap.g, normMap.b * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 reflectedLight = reflect(normalize(-dirLight.direction), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = dirLight.specular * specular * reflectivity;



	// If above water to fresnel effect
	out_Color = mix(reflectColour, refractColour, refractiveFactor);



	// If underwater, only do refraction
	if(toCameraVector.y < 0)
	{
		out_Color = refractColour;
	}


	// Add ocean colour
	out_Color = mix(out_Color, oceanColour, oceanBlend);

	out_Color += vec4(specularHighlights, 1.0);
}
