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
  glm::vec3 * translations;
  int max;
  Shader * shader = new Shader("fish.vs", "fish.fs");


  Fish(int max)
  {
    this->max = max;
    translations = (glm::vec3 *) malloc(max * sizeof(glm::vec3));

    setupFish();
  }

  void setupFish()
  {
    texture = loadTexture(FileSystem::getPath("resources/textures/allFish.png").c_str(), 0);

    float randX = (float)(rand()) / RAND_MAX;
    float randY = (float)(rand()) / RAND_MAX;
    for (int i = 0; i < max; i++)
    {

      translations[i] = glm::vec3((float)(i%10) + randX, 0., (i/10) + randY);
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * max, &translations[0], GL_STATIC_DRAW);
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
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
    float speed = 50.;
    for (int i = 0; i < max; i++)
    {
      p = glm::perlin(glm::vec2(translations[i].x, translations[i].y));
      translations[i].z += sin(3.14159 * p)/speed;
      translations[i].x += cos(3.14159 * p)/speed;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * max, &translations[0], GL_STATIC_DRAW);
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
