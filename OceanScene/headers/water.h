#ifndef WATER_H
#define WATER_H

#include "shader.h"
#include "glm.hpp"
#include "camera.h"
#include "waterTile.h"
#include "waterFrameBuffers.h"

class Water
{
public:

  Shader * shader = new Shader("waterVertex.vs", "waterFragment.fs");
  float vertices[12] = { -1., -1., -1., 1., 1., -1., 1., -1., -1., 1., 1., 1. };
  WaterFrameBuffers * fbos;

  unsigned int VAO, VBO;
  float size;

  Water(WaterFrameBuffers * fbos)
  {
    this->fbos = fbos;
    setupShader();
  }

  void Draw(WaterTile * water, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera camera, glm::vec4 clipPlane, float size)
  {
    // Activate Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbos->getRefractionTexture());

    // Translate to water position
    model = glm::translate(model, glm::vec3(water->x, water->height, water->z));
    model = glm::scale(model, glm::vec3(size, size, size));

    // Set Uniforms
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    shader->setVec4("clipPlane", clipPlane);

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

    // Set texture var in shader
    shader->use();
    shader->setInt("reflectionTexture", 0);
    shader->setInt("refractionTexture", 1);

  }
};



#endif // WATER_H
