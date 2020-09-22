#ifndef SEAWEED_H
#define SEAWEED_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <time.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"
#include "filesystem.h"
#include "texturesSetup.h"
#include "verticies.h"

#include "shader.h"

#include <string>
#include <vector>
using namespace std;



class Seaweed {
public:

    unsigned int VAO, VBO, texture;
    Shader* shader;
    int seaweedNum;
    glm::vec3 * translations;


    // constructor
    Seaweed(int maxSeaweed, glm::vec3 *positions)
    {

      seaweedNum = maxSeaweed;
      texture = loadTexture(FileSystem::getPath("resources/textures/allSeaweed.png").c_str());

      this->shader = new Shader("seaweed.vs", "seaweed.fs");
      // Malloc for translations
      translations = positions;


      // Do all the OpenGL setup
      setupSeaweed();
    }

    // render the mesh
    void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time)
    {
        // Activate Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Pass Shader Args
        shader->use();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setFloat("time", time);

        // Draw
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, seaweedNum);
        glBindVertexArray(0);

    }

private:

    // initializes all the buffer objects/arrays
    void setupSeaweed()
    {
      unsigned int instanceVBO;
      glGenBuffers(1, &instanceVBO);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * seaweedNum, &translations[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticies), quadVerticies, GL_STATIC_DRAW);
      glBindVertexArray(VAO);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glVertexAttribDivisor(2, 1);

      // Set texture var in shader
      shader->use();
      shader->setInt("texture1", 0);
    }
};

#endif
