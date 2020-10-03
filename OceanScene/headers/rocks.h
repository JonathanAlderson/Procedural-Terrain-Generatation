#ifndef ROCKS_H
#define ROCKS_H


#include "rock.h"
#include "filesystem.h"
#include "texturesSetup.h"
#include <vector>

class Rocks
{
public:

  float nrVertices;
  float length;
  std::vector<glm::vec3> rockPositions;
  float isoLevel;
  float noiseScale;
  int genType;
  float totalVertices;
  float nrCubes;
  float totalCubes;
  unsigned int texture, normalMap;

  Shader* rockShader = new Shader("rock.vs", "rock.fs");

  Rock **rocks;


  Rocks(float nVertices, float nLength, std::vector<glm::vec3> rockPositions, float isoLevel, float noiseScale, int genType)
  {
    this->rockPositions = rockPositions;
    this->nrVertices = nVertices;
    this->totalVertices = nrVertices * nrVertices * nrVertices;
    this->nrCubes = nrVertices - 1;
    this->totalCubes = (nrVertices - 1) * (nrVertices - 1) * (nrVertices - 1);
    this->length = nLength;
    this->noiseScale = noiseScale;
    this->isoLevel = isoLevel;
    this->genType = genType;

    rocks = (Rock **) malloc(rockPositions.size() * sizeof(Rock*));
    for(unsigned int i = 0; i < rockPositions.size(); i++)
    {
      std::cout << "Pos: " << rockPositions[i].x << " " << rockPositions[i].z << '\n';
      rocks[i] = new Rock(nrVertices, length * (.5 + ((float)i/(float)rockPositions.size())), rockPositions[i], isoLevel, noiseScale, genType);

    }

    texture = loadTexture(FileSystem::getPath("resources/textures/UnderwaterRockTexture.png").c_str(), 1);
    normalMap = loadTexture(FileSystem::getPath("resources/textures/UnderwaterRockNormal.png").c_str(), 1);

    setupShader();
  }


  void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 camPos, glm::vec4 clipPlane)
  {
    // Activate Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    rockShader->use();
    rockShader->setMat4("projection", projection);
    rockShader->setMat4("view", view);
    rockShader->setMat4("model", model);
    rockShader->setVec3("viewPos", camPos);
    rockShader->setVec4("clipPlane", clipPlane);

    //rockShader->setVec3("dirLight.lightPos", camPos.x, camPos.y, camPos.z);
    for(unsigned int i = 0; i < rockPositions.size(); i++)
    {
      glBindVertexArray(rocks[i]->VAO);
      glDrawArrays(GL_TRIANGLES, 0, rocks[i]->triangleDataSize);
    }

    // normalShader->use();
    // normalShader->setMat4("projection", projection);
    // normalShader->setMat4("view", view);
    // normalShader->setMat4("model", model);
    //
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, triangleData.size());
  }

  void setupShader()
  {
    rockShader->use();
    rockShader->setInt("diffuseTexture", 0);
    rockShader->setInt("normalTexture", 1);
    rockShader->setVec3("fragCol", glm::vec3(124.0f, 124.0f, 124.0f));
    rockShader->setVec3("dirLight.lightPos", 100.0f, 100.0f, 100.0f);
    rockShader->setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
    rockShader->setVec3("dirLight.diffuse", 1.f, 1.f, 1.f);
    rockShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
  }
};

#endif // ROCKS_H
