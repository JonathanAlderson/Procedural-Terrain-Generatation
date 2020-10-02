#ifndef BEIZER_H
#define BEIZER_H

#include "glm.hpp"
#include <vector>
#include <iostream>


class BCurve
{
public:

  std::vector<glm::vec3> out;       // Precalculated Points
  std::vector<glm::vec3> pointsCopy;// Input Points
  float t;                          // Current Time
  int steps;                        // How many points calculated

  BCurve(std::vector<glm::vec3> pointsIn, int steps)
  {
     // Go through every step and calculate final point
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

  glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t)
  {
    return glm::vec3(a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t, a.z + (b.z-a.z)*t);
  }


  glm::vec3 get(float t)
  {
    // Interpolate between pre computed values
    t = std::min(t, 1.0f);
    int index1 = int(floor(t /  (1./((float)steps-1.))));
    int index2 = index1 + 1;

    float t1 = index1 * (1. / ((float)steps-1.));
    float t2 = index2 * (1. / ((float)steps-1.));

    // Distance to interpolate
    float c = (t - t1) / (t2 - t1);

    return lerp(out[index1], out[index2], c);

  }

  std::vector<glm::vec3> calc(std::vector<glm::vec3> points, float t)
  {
    // Scary Magic
    std::vector<glm::vec3> ret;
    for(unsigned int i = 0; i < points.size() - 1; i++)
    {
      ret.push_back(lerp(points[i], points[i+1], t));
    }
    return ret;
  }
};

#endif // BEIZER_H
