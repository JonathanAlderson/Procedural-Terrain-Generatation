#version 330 core
out vec4 FragColor;

in vec3 TexCoords;


uniform samplerCube skybox;
uniform vec3 viewPos;
uniform float planeSize;


void main()
{

    vec3 viewRay = TexCoords;
    vec3 interceptPoint;
    float dist;

    // if below and looking up or above and looking down
    if((viewPos.y > 0 && viewRay.y < 0) || (viewPos.y < 0 && viewRay.y > 0))
    {
       // Distance To Plane
      dist = -viewPos.y / viewRay.y;
      // Where ray meets plane
      interceptPoint = vec3(viewPos.x + dist * viewRay.x, viewPos.y + dist * viewRay.y, viewPos.z + dist * viewRay.z);

      // Check if outside the plane
      if(interceptPoint.x > planeSize || interceptPoint.x < -planeSize || interceptPoint.z > planeSize || interceptPoint.z < -planeSize)
      {
        if(viewPos.y > 0)
        {

          FragColor = mix(texture(skybox, TexCoords), vec4(112./255., 214./255., 255./255., 1.), 0.2);
        }
        else
        {
          FragColor = vec4(0.243, 0.573, .8, 1.); // sea colour
        }
        return;
      }

    }
    if(viewPos.y < 0  && TexCoords.y < 0 )
    {
      FragColor = vec4(0.243, 0.573, .8, 1.);
    }
    else
    {
      FragColor = texture(skybox, TexCoords);
    }






}
