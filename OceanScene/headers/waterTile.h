#ifndef WATERTILE_H
#define WATERTILE_H

#include "shader.h"

class WaterTile
{
public:

  float x, z;
  float height;

  WaterTile(float centerX, float centerZ, float height)
  {
    this->x = centerX;
    this->z = centerZ;
    this->height = height;
  }

  float getHeight()
  {
    return height;
  }

  float getX()
  {
    return x;
  }

  float getZ()
  {
    return z;
  }


};



#endif // WATER_H
