#ifndef WATER_H
#define WATER_H

#include "shader.h"
#include "glm.hpp"
#include "camera.h"
#include "waterTile.h"
#include "waterFrameBuffers.h"
#include "filesystem.h"
#include "texturesSetup.h"

class Water
{
public:

  Shader * shader = new Shader("waterVertex.vs", "waterFragment.fs");
  float vertices[12] = { -1., -1., -1., 1., 1., -1., 1., -1., -1., 1., 1., 1. };
  WaterFrameBuffers * fbos;

  unsigned int VAO, VBO, dudvMap, normalMap;
  float size;

  Water(WaterFrameBuffers * fbos)
  {
    this->fbos = fbos;
    dudvMap = loadTexture(FileSystem::getPath("resources/textures/waterDudv.png").c_str(), 1);
    normalMap = loadTexture(FileSystem::getPath("resources/textures/normalMap.png").c_str(), 1);
    setupShader();
  }

  void Draw(WaterTile * water, glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time, Camera camera, glm::vec4 clipPlane, float size)
  {
    // Activate Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbos->getRefractionTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dudvMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, fbos->getRefractionDepthTexture());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Translate to water position
    model = glm::translate(model, glm::vec3(water->x, water->height, water->z));
    model = glm::scale(model, glm::vec3(size, size, size));

    // Set Uniforms
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    shader->setVec4("clipPlane", clipPlane);
    shader->setFloat("time", time);
    shader->setVec3("cameraPosition", camera.Position);
    shader->setFloat("near", glm::radians(camera.Zoom));
    shader->setFloat("far", 1000.);

    // Load and Render
    glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
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
    shader->setInt("dudvMap", 2);
    shader->setInt("normalMap", 3);
    shader->setInt("depthMap", 4);

    // directional light
    shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLight.diffuse", 1.f, 1.f, 1.f);
    shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

  }
};



#endif // WATER_H
