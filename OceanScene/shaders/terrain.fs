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
uniform vec3 viewPos;
uniform DirLight dirLight;

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseCol, vec3 specCol)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * diffuseCol;
    vec3 diffuse = light.diffuse * diff * diffuseCol;
    vec3 specular = light.specular * spec * specCol;

    //return specular;
    return (ambient + diffuse + specular);
}


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffuseCol = vec3(1., 0., 0.);
    vec3 specCol = vec3(1., 1., 1.);


    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffuseCol, specCol);

    FragColor = vec4(result, 1.0);


}
