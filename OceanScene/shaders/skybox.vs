#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 clipPlane;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
    gl_ClipDistance[0] = dot(vec4(aPos, 1.), clipPlane); //(projection * vec4(aPos, 1.0)).y;//dot(movedModel * vec4(vert, 1.0) , clipPlane);

}
