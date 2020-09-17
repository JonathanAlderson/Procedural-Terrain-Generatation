#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "sceneSetup.h"
#include "verticies.h"


// Global Defenitions For Scene Setup
unsigned int cubeVBO, cubeVAO;
unsigned int lightCubeVAO;
unsigned int seaweedVAO, quadVBO;

glm::vec3 cubePositions[10] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 pointLightPositions[4] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


glm::vec3 seaweedPositions[5] = {
    glm::vec3( 0.2f,  0.0f,  0.2f),
    glm::vec3( 1.2f,  0.0f,  0.2f),
    glm::vec3( 0.4f,  0.0f,  0.4f),
    glm::vec3( 1.6f,  0.0f,  0.6f),
    glm::vec3( 1.0f,  0.0f,  0.8f)
};



// Call All Of The Setup Functions

void sceneSetup()
{
  cubesSetup();
  lightsSetup();
  seaweedSetup();
}

void cubesSetup()
{

  glGenVertexArrays(1, &cubeVAO);
  glBindVertexArray(cubeVAO);
  glGenBuffers(1, &cubeVBO);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void lightsSetup()
{
  glGenVertexArrays(1, &lightCubeVAO);
  glBindVertexArray(lightCubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  // note that we update the lamp's position attribute's stride to reflect the updated buffer data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
}

void seaweedSetup()
{
  // Get the random positions of the seaweed
  glm::vec3 translations[100];
  for (unsigned int i = 0; i < 100; i++)
  {
    translations[i] = glm::vec3((float)(i%10), 0., i/10);
  }

  unsigned int instanceVBO;
  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 100, &translations[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &seaweedVAO);
  glBindVertexArray(seaweedVAO);
  glGenBuffers(1, &quadVBO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticies), quadVerticies, GL_STATIC_DRAW);
  glBindVertexArray(seaweedVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribDivisor(2, 1);



}
