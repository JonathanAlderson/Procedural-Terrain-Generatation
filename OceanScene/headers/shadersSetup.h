#ifndef SHADERSETUP
#define SHADERSETUP
#include "sceneSetup.h"
#include "shader.h"
#include "camera.h"

extern void lightingShaderSetup(Shader shader, Camera camera);
void terrainShaderSetup(Shader terrainShader, float terrainHeight);

#endif
