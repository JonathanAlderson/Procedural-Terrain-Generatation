#version 330 core

out vec4 FragColor;
in vec2 textureCoords;

void main()
{
  //FragColor = texture(colourTexture, textureCoords);

  FragColor = vec4(1.);
}
