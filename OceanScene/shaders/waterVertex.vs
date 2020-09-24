#version 330 core

in vec2 position;
out vec4 clipSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 clipPlane;


void main(void) {

	clipSpace = projection * view * model* vec4(position.x, 0.0, position.y, 1.0);
	gl_Position = clipSpace;

	// Set the clipping plane
	gl_ClipDistance[0] = dot(model * vec4(position.x, 0.0, position.y, 1.0) , clipPlane);


}
