#ifndef BEIZER_H
#define BEIZER_H

#include "glm.hpp"
#include <vector>
#include <iostream>

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t)
{
  return glm::vec3(a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t, a.z + (b.z-a.z)*t);
}

class BCurve
{
public:

  std::vector<glm::vec3> out;
  std::vector<glm::vec3> pointsCopy;
  float t;
  int steps;

  BCurve(std::vector<glm::vec3> pointsIn, int steps)
  {
     this->steps = steps;
     for(int j = 0; j < steps; j ++)
     {
       t = (1./((float)steps-1.)) * (float)j;
       pointsCopy = pointsIn; // reset
       for(unsigned int i = 0; i < pointsCopy.size(); i++)
       {
         pointsCopy = calc(pointsCopy, t);
       }
       out.push_back(pointsCopy[0]);
     }
  }

  glm::vec3 get(float t)
  {
    int index = round(t /  (1./((float)steps-1.)));

    return out[index];

  }

  std::vector<glm::vec3> calc(std::vector<glm::vec3> points, float t)
  {
    std::vector<glm::vec3> ret;
    for(unsigned int i = 0; i < points.size() - 1; i++)
    {
      ret.push_back(lerp(points[i], points[i+1], t));
    }
    return ret;
  }
};

#endif // BEIZER_H
