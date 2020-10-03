#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 textureCoords;


struct DirLight {
    vec3 lightPos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform vec3 fragCol;
uniform DirLight dirLight;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseCol, vec3 specCol, float shine)
{
    vec3 lightDir = normalize(light.lightPos - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shine);
    // combine results
    vec3 ambient = light.ambient * diffuseCol;
    vec3 diffuse = light.diffuse * diff * diffuseCol;
    vec3 specular = light.specular * spec * specCol;

    //return specular;
    return ( ambient + diffuse +  specular);
}

vec3 rgb(float r, float g, float b)
{
  return vec3(r / 255., g / 255., b / 255.);
}

void main()
{
    vec3 normMap = texture(normalTexture, textureCoords).xyz;
    vec3 normal = vec3(normMap.r * 2.0 - 1.0, normMap.g * 3.0, normMap.b * 2.0 - 1.0);
    vec3 norm = normalize(Normal + normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shine = 50.0f;

    vec3 diffuseCol = texture(diffuseTexture, textureCoords).xyz;

    vec3 specCol = vec3(1.0, 1.0, 1.0);

    vec4 result = vec4(CalcDirLight(dirLight, norm, viewDir, diffuseCol, specCol, shine), 1.0);

    // Make bluer as get further away
    if(FragPos.y < 0.)
    {
      result = mix(result, vec4(0.243, 0.573, .8, 1.), 1. - min(gl_FragCoord.w * 20., 1.0));
    }

    FragColor = result;

}
