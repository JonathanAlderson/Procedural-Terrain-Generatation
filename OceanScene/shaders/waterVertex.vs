#version 330 core

in vec2 position;
out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCameraVector;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 clipPlane;
uniform vec3 cameraPosition;



const float tiling = 6.0;

void main(void) {

	vec4 worldSpace = model * vec4(position.x, 0.0, position.y, 1.0);

	clipSpace = projection * view * worldSpace;
	gl_Position = clipSpace;

	// Set the clipping plane
	gl_ClipDistance[0] = dot(worldSpace , clipPlane);

	// for duvu
	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * tiling;

	// for frenel
	toCameraVector = cameraPosition - worldSpace.xyz;

}
