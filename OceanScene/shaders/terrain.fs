#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;


struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

vec3 colour()
{
  vec3 sand = rgb(251., 244., 157.);
  vec3 grass = rgb(124., 124., 124.);
  vec3 rock = rgb(153., 170., 56.);

  float height = FragPos.y;

  if(height < 0.)
  {
    return sand;
  }
  if(height < .1)
  {
    return(sand * (1. - height*10.) * grass * (height*10.));
  }

  return vec3(0., 0., 0.);


  return vec3(FragPos.y, 0., 0.);
  return rgb(maxHeight, 0. , 0.);

}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shine = dot(Normal, vec3(0., 1., 0.))*500.;

    vec3 diffuseCol = colour();

    vec3 specCol = vec3(1., 1., 1.);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffuseCol, specCol, shine);

    FragColor = vec4(result, 1.0);


}
