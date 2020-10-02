#ifndef FISH_H
#define FISH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "verticies.h"
#include "gtc/noise.hpp"
#include "glm.hpp"
#include "filesystem.h"
#include "texturesSetup.h"
#include "shader.h"

class Fish
{
public:

  unsigned int VBO, VAO, quadVBO, texture;
  glm::vec4 * translations;
  int max;
  int schoolSize;
  float oceanSize;
  float * seeds;
  Shader * shader = new Shader("fish.vs", "fish.fs");


  Fish(int max, int schoolSize, float oceanSize)
  {
    this->max = max;
    this->schoolSize = schoolSize;
    this->oceanSize = oceanSize;
    seeds = (float *) malloc(sizeof(float) * (max / schoolSize));
    translations = (glm::vec4 *) malloc(max * sizeof(glm::vec4));

    setupFish();
  }

  float randFloat()
  {
    return (float)(rand()) / RAND_MAX;
  }

  void setupFish()
  {
    texture = loadTexture(FileSystem::getPath("resources/textures/allFish.png").c_str(), 0);

    float x;
    float y;
    float z;

    for (int i = 0; i < max/schoolSize; i++)
    {
      x = (randFloat()-.5)*oceanSize*2.0;
      z = (randFloat()-.5)*oceanSize*2.0;
      y = -1 + -randFloat() * 5.;
      for(int j = 0; j < schoolSize; j ++)
      {
        x += (randFloat()-.5)*.5;
        y += (randFloat()-.5)*.5;
        z += (randFloat()-.5)*.5;
        translations[j + i*schoolSize] = glm::vec4(x, y, z, 0.);
      }

      seeds[i] = rand()%10000;
    }



    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * max, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticies), quadVerticies, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);

    // Set texture var in shader
    shader->use();
    shader->setInt("texture1", 0);

  }

  void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time, glm::vec4 clipPlane)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    float p;
    float speed = .02;
    float scale = .2;
    int j;

    for (int i = 0; i < max; i++)
    {
      j = i/schoolSize;
      p = glm::perlin(glm::vec2((translations[i].x + translations[i].y)*scale + seeds[j], (translations[i].z+ + translations[i].y)*scale + seeds[j]));
      translations[i].z += sin(2. * 3.14159 * p)*speed;
      translations[i].x += cos(2. * 3.14159 * p)*speed;
      translations[i].y += ((randFloat()-.5)*.5)*speed;
      if(translations[i].y > -1.){translations[i].y -= 0.01; }
      translations[i].w = 2. * 3.14159 * p; // set the rotation

      //std::cout << "Fish: " << translations[i].x << " " << translations[i].z  << " --> " << p << std::endl;


      if(translations[i].x < -oceanSize || translations[i].x > oceanSize || translations[i].z < -oceanSize|| translations[i].z > oceanSize)
      {
        translations[i].x = 0.;
        translations[i].z = 0;
      }

    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * max, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

     shader->use();
     shader->setMat4("model", model);
     shader->setMat4("view", view);
     shader->setMat4("projection", projection);
     shader->setFloat("time", time);
     shader->setVec4("clipPlane", clipPlane);


     glBindVertexArray(VAO);
     glDrawArraysInstanced(GL_TRIANGLES, 0, 6, max);
     glBindVertexArray(0);

  }


};

#endif // FISH_H
