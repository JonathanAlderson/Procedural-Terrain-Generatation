#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clipPlane;

out vec3 FragPos;
out vec3 Normal;

void main()
{
	Normal = mat3(transpose(inverse(model))) * aNormal;
	//Normal = normalize(aNormal);
  FragPos = vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * model * view * vec4(aPos, 1.0f);

	// Set the clipping plane
	gl_ClipDistance[0] = 1.; //dot(model * vec4(aPos, 1.0) , clipPlane);
}
