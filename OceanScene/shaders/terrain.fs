#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform sampler2D sandNormalMap;
uniform sampler2D grassNormalMap;
uniform sampler2D rockNormalMap;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform float time;
uniform float shininess;
uniform float maxHeight;
uniform vec3 viewPos;
uniform DirLight dirLight;


// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseCol, vec3 specCol, float shine)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);
    // combine results
    vec3 ambient = light.ambient * diffuseCol;
    vec3 diffuse = light.diffuse * diff * diffuseCol;
    vec3 specular = light.specular * spec * specCol;

    //return specular;
    return (ambient + diffuse + specular);
}

vec3 rgb(float r, float g, float b)
{
  return vec3(r / 255., g / 255., b / 255.);
}

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

vec3 colour(float height, float sandLower, float sandUpper, float grassLower, float grassUpper)
{
  float hMultiplier;
  vec3 sand = rgb(251., 244., 157.);
  vec3 grass = rgb(153., 170., 56.);
  vec3 rock = rgb(124., 124., 124.);

  vec3 col = .2 * vec3(noise(Normal.y), noise(FragPos.x*0.043), noise(FragPos.z*.035));

  if(height < 0.)
  {
    return col + sand;
  }
  if(height < sandUpper)
  {
    hMultiplier = (height - sandLower)/(sandUpper - sandLower);
    return col + (sand * (1. - hMultiplier) + grass * hMultiplier);
  }
  if(height < grassUpper)
  {
    hMultiplier = (height - grassLower)/(grassUpper - grassLower);
    return col + (grass * (1.-hMultiplier) + rock * hMultiplier);
  }

  return col + rock;
}

// Find the correct normal map based on height
vec4 getNormalMap(float height, float sandLower, float sandUpper, float grassLower, float grassUpper)
{
  float hMultiplier;

  if(height < 0.)
  {
    return texture(sandNormalMap, vec2(FragPos.x*.2, FragPos.z*.2));
  }
  if(height < sandUpper)
  {
    hMultiplier = (height - sandLower)/(sandUpper - sandLower);
    return (1. -hMultiplier) * texture(sandNormalMap, vec2(FragPos.x*.2, FragPos.z*.2)) + hMultiplier * texture(grassNormalMap, vec2(FragPos.x*.2, FragPos.z*.2));
  }
  if(height < grassUpper)
  {
    hMultiplier = (height - grassLower)/(grassUpper - grassLower);
    return (1. -hMultiplier) * texture(grassNormalMap, vec2(FragPos.x*.2, FragPos.z*.2)) + hMultiplier * texture(rockNormalMap, vec2(FragPos.x*.2, FragPos.z*.2));
  }

  return texture(rockNormalMap, vec2(FragPos.x*.2, FragPos.z*.2));;

}

// Give nice patterns on the sand
vec3 caustics()
{
  float tau = 6.28318530718;
  int max_iter = 4;

  float cTime = time * .5+23.0;
    // uv should be the 0-1 uv of texture...
	vec2 uv = FragPos.xz * .1;

  uv.x += time * .1;

	vec2 p = mod(uv*tau*2.0, tau)-250.0;

	vec2 i = vec2(p);
	float c = 1.0;
	float inten = .002;

	for (int n = 0; n < max_iter; n++)
	{
		float t = cTime * (1.0 - (3.5 / float(n+1))) + FragPos.x + FragPos.z;
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
	c /= float(max_iter);
	c = 1.17-pow(c, 1.4);
	vec3 colour = vec3(pow(abs(c), 8.0));

  // Only once you get underwater
  colour *= smoothstep(0, -10., FragPos.y);

	return colour;
}


void main()
{

    // Globals


    float height = FragPos.y/maxHeight;


    float sandLower = 0.;
    float sandUpper = .05;

    float grassLower = 0.05;
    float grassUpper = 0.6;





    // normals and highlights
    vec4 normMap = getNormalMap(height, sandLower, sandUpper, grassLower, grassUpper);
    vec3 norm = vec3(normMap.r * 2.0 - 1.0, normMap.g * 3.0, normMap.b * 2.0 - 1.0);
    norm = normalize(Normal + norm);

    //vec3 norm = normalize(Normal);


    vec3 viewDir = normalize(viewPos - FragPos);

    float shine = dot(Normal, vec3(0., 1., 0.))*1000.;

    shine = 3000. * (2.5 - height);



    vec3 diffuseCol = colour(height, sandLower, sandUpper, grassLower, grassUpper);

    vec3 specCol = vec3(1., 1., 1.);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffuseCol, specCol, shine);

    result += caustics();

    // Make things darker underwater
    if(height < 0)
    {
      result = mix(result, vec3(0.243, 0.573, .8), 1. - min(gl_FragCoord.w * 20., 1.0));
    }

    FragColor = vec4(result, 1.0);


}
