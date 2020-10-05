#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec2 textureCoords;

void main()
{
  //gl_Position = vec4(aPos, 1.0);

  gl_Position = vec4(.5, .5, .0, 1.0);

  textureCoords = aTexCoord;

//textureCoords = aTexCoords * .5 + .5;
}
