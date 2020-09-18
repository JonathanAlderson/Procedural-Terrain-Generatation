#ifndef SCENESETUP_H
#define SCENESETUP_H
#include "glm.hpp"


//////////////////////////////////
// Scene Globals
/////////////////////////////////
extern unsigned int cubeVBO, cubeVAO;
extern unsigned int lightCubeVAO;
extern unsigned int seaweedVAO, quadVBO;
extern unsigned int terrainVBO, terrainVAO, terrainEBO;
extern unsigned int fishVAO, fishVBO;


// Individual Setup Functions
void cubesSetup();
void lightsSetup();
extern void seaweedSetup(int max);
glm::vec3 * fishSetup(int max, glm::vec3 * translations);
glm::vec3 * updateFish(int max, glm::vec3 * translations);
void terrainSetup();


// Scene Setup Header
// positions all containers
extern glm::vec3 cubePositions[10];

// lighting
extern glm::vec3 lightPos;

// positions of the point lights
extern glm::vec3 pointLightPositions[4];

#endif
