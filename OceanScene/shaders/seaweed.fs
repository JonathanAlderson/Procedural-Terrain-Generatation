#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in float ID;

uniform sampler2D texture1;
uniform float time;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
  // Create random variables
  float r = rand(vec2(ID, ID));
  float smoothR = smoothstep(0., 1., r);


  float sway = 0.01 + 0.02 * smoothR;
  float freq = 1. + 3. * smoothR;
  float offset = r;
  float speed = 1. + 2. * smoothR; //10.0 * (r+.5);


  // create a random piece of seaweed
  float seaweedID = float(int(r * 10.));
  vec2 tex = TexCoords;
  tex.x += seaweedID/10.;


  // calculate the waving effect
  float xOff = sin(tex.y*freq + time*speed + offset);
  xOff *= sway;

  // make it less prevelent at the bottom
  float prevelence = smoothstep(1.0, 0.0, tex.y);

  xOff *= prevelence;
  tex.x += xOff;


  vec4 texColor = texture(texture1, tex);
  if(texColor.a < 0.1)
      discard;

  FragColor = mix(texColor, vec4(0.463, 0.714, 0.768, 1.), 1. - min(gl_FragCoord.w * 10., 1.0));

}
