#ifndef VERTICIES_H
#define VERTICIES_H

#include "glm.hpp"

extern float containerVertices[288];
extern float quadVerticies[30];

namespace v
{
  struct Vertex
  {
      // position
      glm::vec3 Position;
      // normal
      glm::vec3 Normal;
  };
}
struct ScalarVertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // scalar
    float Scalar;
};

extern int edgeTable[256];
extern int triTable[256][16];
extern float skyboxVertices[108];
#endif
