#ifndef WATER_H
#define WATER_H

#include "shader.h"
#include "glm.hpp"
#include "camera.h"
#include "waterTile.h"

class Water
{
public:

  Shader * shader = new Shader("waterVertex.vs", "waterFragment.fs");
  float vertices[12] = { -1., -1., -1., 1., 1., -1., 1., -1., -1., 1., 1., 1. };

  unsigned int VAO, VBO;

  Water()
  {
    setupShader();
  }

  void Draw(WaterTile * water, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera camera)
  {

    // Translate to water position
    model = glm::translate(model, glm::vec3(water->x, water->height, water->z));

    // Set Uniforms
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);

    // Load and Render
    glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

  }

  void setupShader()
  {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    // bind vertex and buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // set vertex attributes' layouts
    // vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

  }
};



#endif // WATER_H
