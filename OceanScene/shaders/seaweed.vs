#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 offsets;

out vec2 TexCoords;
out float ID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Code for rotatings
mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


void main()
{
    // Move the model
    mat4 movedModel = model;
    vec3 pos = offsets;
    movedModel[3][0] = pos.x; //pos.x;
    movedModel[3][1] = pos.y;//pos.y;
    movedModel[3][2] = pos.z; //pos.z;

    // Rotate the seaweed randomly
    mat4 r = rotationMatrix(vec3(0., 1., 0.), gl_InstanceID);
    movedModel = movedModel * r;

    // Scale the seaweed randomly
    vec3 vert = aPos;
    float scale = 0.2 + 1.5 * rand(vec2(gl_InstanceID, 36.7));
    vert = vert * scale;

    // Adjust the height after scaling
    movedModel[3][1] += scale;//pos.y;


    // Outputs
    ID = gl_InstanceID;
    TexCoords = aTexCoords;
    gl_Position = projection * view * movedModel * vec4(vert, 1.0);
}
