#ifndef SHADERSETUP
#define SHADERSETUP
#include "sceneSetup.h"
#include "shader.h"
#include "camera.h"

extern void lightingShaderSetup(Shader shader, Camera camera);
extern void terrainShaderSetup(Shader terrainShader);

#endif
