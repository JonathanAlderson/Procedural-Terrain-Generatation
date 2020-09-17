#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 offsets;

out vec2 TexCoords;
out float ID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

void main()
{
    // Move the model
    mat4 movedModel = model;
    vec3 pos = offsets;
    movedModel[3][0] = pos.x; //pos.x;
    movedModel[3][1] = pos.y;//pos.y;
    movedModel[3][2] = pos.z; //pos.z;


    // Outputs
    ID = gl_InstanceID;
    TexCoords = aTexCoords;
    gl_Position = projection * view * movedModel * vec4(aPos, 1.0);
}
