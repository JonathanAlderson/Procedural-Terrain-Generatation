#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clipPlane;

out vec3 FragPos;
out vec3 Normal;
out vec2 textureCoords;

void main()
{
	Normal = mat3(transpose(inverse(model))) * normalize(aNormal);
	//Normal = normalize(aNormal);
  FragPos = vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * model * view * vec4(aPos, 1.0f);

	// Set the clipping plane
	gl_ClipDistance[0] = dot(model * vec4(aPos, 1.0) , clipPlane);

	// for texturing

	// TODO
	// put the normal in here
	textureCoords = vec2(aPos.x + aPos.y, aPos.z) * .2;
}
