#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out float colourSea;
out vec3 viewDirection;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 clipPlane;
uniform vec3 viewPos;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
    gl_ClipDistance[0] = dot(vec4(aPos, 1.), clipPlane); //(projection * vec4(aPos, 1.0)).y;//dot(movedModel * vec4(vert, 1.0) , clipPlane);

    colourSea = 0.;

    // Where the view ray meets the sea
    vec3 interceptPoint;
    float dist;

    colourSea = 0.;

    viewDirection = pos.xyz;

    dist = -viewPos.y / pos.y;
    if(dist > 0)
    {
      interceptPoint = vec3(viewPos.x + dist * pos.x, viewPos.y + dist * pos.y, viewPos.z + dist * pos.z);
    }

    viewDirection = vec3(dist);
    /* if(viewPos.y < 0 && pos.y > 0)
    {
      dist = -pos.y / viewPos.y;

      if(dist > 0)
      {
        interceptPoint = vec3(viewPos.x + dist * pos.x, viewPos.y + dist * pos.y, viewPos.z + dist * pos.z);
      }

      if(interceptPoint.x > 100. && interceptPoint.z > 100.)
      {
        colourSea = 1.0;
      }

    } */



}
