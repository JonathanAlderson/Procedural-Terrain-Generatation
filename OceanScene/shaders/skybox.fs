#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 viewPos;


void main()
{

    FragColor = texture(skybox, TexCoords);
    if(viewPos.y < 0)
    {
        if(TexCoords.y < 0)
        {
          FragColor = vec4(0.243, 0.573, .8, 1.0); //, 1. - min(gl_FragCoord.w * 20., 1.0));
        }
    }

}
