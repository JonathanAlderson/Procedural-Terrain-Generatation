#version 330 core

out vec4 FragColor;
in vec2 textureCoords;

uniform sampler2D colourTexture;
uniform sampler2D depthTexture;

const float constrast = 0.25;

void main()
{
  FragColor = vec4(0., texture(colourTexture, textureCoords).gba);
  //FragColor.rgb = (FragColor.rgb - 0.5) * (1.0 + constrast) + 0.5;

  FragColor += vec4(texture(depthTexture, textureCoords).r, 0., 0., 1.0);

}
