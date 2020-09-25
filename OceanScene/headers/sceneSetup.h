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


// Individual Setup Functions
void cubesSetup();
void lightsSetup();
void terrainSetup();


// Scene Setup Header
// positions all containers
extern glm::vec3 cubePositions[10];

// lighting
extern glm::vec3 lightPos;

// positions of the point lights
extern glm::vec3 pointLightPositions[4];

#endif
