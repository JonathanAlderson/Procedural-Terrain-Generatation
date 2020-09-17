#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform float time;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
  float sway = 0.003;
  float freq = 3.0;


  // create a random piece of seaweed
  float seaweedID = 2.;
  vec2 tex = TexCoords;
  tex.x += seaweedID/10.;


  // calculate the waving effect
  float xOff = sin(tex.y*freq + time);
  xOff *= sway;

  // make it less prevelent at the bottom
  float prevelence = smoothstep(1.0, 0.8, tex.y);

  xOff *= prevelence;


  tex.x += xOff;


  vec4 texColor = texture(texture1, tex);
  if(texColor.a < 0.1)
      discard;
  FragColor = texColor;
}
