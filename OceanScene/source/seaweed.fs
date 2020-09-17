#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

void main()
{
  //vec3(texture(material.diffuse, TexCoords));
  FragColor = vec4(1.0);
}
