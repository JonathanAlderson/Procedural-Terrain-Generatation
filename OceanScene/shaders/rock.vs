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
	// makes a good guess at the texure coords

	/* float hAngle = dot(Normal.xz, vec2(1., 0.));
	float vAngle = dot(Normal.yz, vec2(1., 0.));

	float texX = aPos.x * sin(hAngle) + aPos.z * cos(hAngle);
	float texY = aPos.z * sin(vAngle) + aPos.y * cos(vAngle); */

	float hAngle = dot(Normal.xz, vec2(1., 0.));

	float texX = aPos.x + cos(hAngle)*aPos.y;
	float texY = aPos.z;// + sin(hAngle)*aPos.y;

	textureCoords = vec2(texX, texY) * .2;
}
