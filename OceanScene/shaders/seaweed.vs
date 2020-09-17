#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 offsets;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
    // Move the model
    mat4 movedModel = model;
    vec3 pos = offsets;

    movedModel[3][0] = pos.x; //pos.x;
    movedModel[3][1] = pos.y;//pos.y;
    movedModel[3][2] = pos.z; //pos.z;

    TexCoords = aTexCoords;
    gl_Position = projection * view * movedModel * vec4(aPos, 1.0);
}
