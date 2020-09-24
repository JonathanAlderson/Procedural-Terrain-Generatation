#ifndef SKYBOX_H
#define SKYBOX_H

#include "shader.h"
#include "verticies.h"
#include "filesystem.h"
#include "camera.h"
#include "stb_image.h"
#include <iostream>
#include <vector>

class Skybox
{
public:
  Shader * shader = new Shader("skybox.vs", "skybox.fs");
  std::vector<std::string> faces;
  unsigned int skyboxVAO, skyboxVBO;
  unsigned int cubemapTexture;

  glm::mat4 view;



  Skybox(std::string skyboxName)
  {

    // Get files
    faces.push_back(FileSystem::getPath("resources/textures/" + skyboxName + "/right.png"));
    faces.push_back(FileSystem::getPath("resources/textures/" + skyboxName + "/left.png"));
    faces.push_back(FileSystem::getPath("resources/textures/" + skyboxName + "/top.png"));
    faces.push_back(FileSystem::getPath("resources/textures/" + skyboxName + "/bottom.png"));
    faces.push_back(FileSystem::getPath("resources/textures/" + skyboxName + "/front.png"));
    faces.push_back(FileSystem::getPath("resources/textures/" + skyboxName + "/back.png"));

    cubemapTexture = loadCubemap(faces);
    setupShader();
  }

  void Draw(glm::mat4 projection, Camera camera)
  {
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    shader->use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
  }

  void setupShader()
  {
    shader->use();
    shader->setInt("skybox", 0);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  }

  unsigned int loadCubemap(std::vector<std::string> faces)
  {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
    }

};

#endif // SKYBOX_H
