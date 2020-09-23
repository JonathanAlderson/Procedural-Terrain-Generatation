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

uniform vec3 viewPos;
uniform vec3 fragCol;
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

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shine = dot(Normal, vec3(0., 1., 0.))*500.;

    vec3 diffuseCol = rgb(fragCol.x, fragCol.y, fragCol.z);

    vec3 specCol = vec3(1., 1., 1.);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffuseCol, specCol, shine);

    FragColor = vec4(result, 1.0);

}