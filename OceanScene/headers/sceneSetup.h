#ifndef SCENESETUP_H
#define SCENESETUP_H
#include "../glm/glm.hpp"


//////////////////////////////////
// Scene Globals
/////////////////////////////////
extern unsigned int VBO, cubeVAO;
extern unsigned int lightCubeVAO;

// Scene Setup Calls All The Below Functions
void sceneSetup();

// Individual Setup Functions
void cubesSetup();
void lightsSetup();


// Scene Setup Header
// positions all containers
extern glm::vec3 cubePositions[10];

// lighting
extern glm::vec3 lightPos;

// positions of the point lights
extern glm::vec3 pointLightPositions[4];

#endif
