#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "verticies.h"
#include "shader.h"

class PostProcessing
{
public:

  unsigned int VBO_texture, VAO_texture, EBO_texture;
  Shader * shader = new Shader("depthOfField.vs", "depthOfField.fs");

// A rectangle which displays the active texture
  float textureVerticies[64] = {
      // positions          // colors           // texture coords
       1.f,  1.f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
       1.f, -1.f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
      -1.f, -1.f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
      -1.f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
  };
  // Order which we render the points
  unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };


  PostProcessing()
  {
    setupPostProcessing();
  }

  void Draw(int texture)
  {
     glClear(GL_COLOR_BUFFER_BIT);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, texture);
     shader->use();

     glBindVertexArray(VAO_texture);
     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  void setupPostProcessing()
  {

    glGenVertexArrays(1, &VAO_texture);
    glGenBuffers(1, &VBO_texture);
    glGenBuffers(1, &EBO_texture);

    glBindVertexArray(VAO_texture);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVerticies), textureVerticies, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_texture);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Set texture var in shader
    shader->use();
    shader->setInt("colourTexture", 0);
  }
};

#endif // POSTPROCESSING_H
