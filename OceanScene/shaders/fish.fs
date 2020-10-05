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


  float sway = .03; //0.01 + 0.02 * smoothR;
  float freq = 15.; //1. + 3. * smoothR;
  float offset = 0; //r;
  float speed = 8.; //1. + 2. * smoothR; //10.0 * (r+.5);


  // create a random fish
  float fishID = float(int(floor(ID/20))%5);
  vec2 tex = TexCoords;
  tex.x *= 2.;
  tex.x += 0.2 * fishID;
  //tex.x += ;



  // calculate the waving effect
  float yOff = sin(tex.x*freq + time*speed)*sway;
  tex.y += yOff;

  vec4 texColor = texture(texture1, tex);
  if(texColor.a < 0.1)
      discard;

  FragColor = mix(texColor, vec4(0.463, 0.714, 0.768, 1.), 1. - min(gl_FragCoord.w * 10., 1.0));
}
