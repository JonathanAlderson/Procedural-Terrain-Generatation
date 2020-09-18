#include "shadersSetup.h"


void lightingShaderSetup(Shader lightingShader, Camera camera)
{
  lightingShader.use();

  lightingShader.setFloat("material.shininess", 32.0f);
  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

  // directional light
  lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
  lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
  lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
  // point light 1
  lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
  lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[0].constant", 1.0f);
  lightingShader.setFloat("pointLights[0].linear", 0.09);
  lightingShader.setFloat("pointLights[0].quadratic", 0.032);
  // point light 2
  lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
  lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[1].constant", 1.0f);
  lightingShader.setFloat("pointLights[1].linear", 0.09);
  lightingShader.setFloat("pointLights[1].quadratic", 0.032);
  // point light 3
  lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
  lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[2].constant", 1.0f);
  lightingShader.setFloat("pointLights[2].linear", 0.09);
  lightingShader.setFloat("pointLights[2].quadratic", 0.032);
  // point light 4
  lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
  lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[3].constant", 1.0f);
  lightingShader.setFloat("pointLights[3].linear", 0.09);
  lightingShader.setFloat("pointLights[3].quadratic", 0.032);
  // spotLight
  lightingShader.setVec3("spotLight.position", camera.Position);
  lightingShader.setVec3("spotLight.direction", camera.Front);
  lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
  lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
  lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("spotLight.constant", 1.0f);
  lightingShader.setFloat("spotLight.linear", 0.09);
  lightingShader.setFloat("spotLight.quadratic", 0.032);
  lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
  lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}



void terrainShaderSetup(Shader terrainShader)
{
  terrainShader.use();

  terrainShader.setFloat("shininess", 32.0f);

  // directional light
  terrainShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
  terrainShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
  terrainShader.setVec3("dirLight.diffuse", 1.f, 1.f, 1.f);
  terrainShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

}
