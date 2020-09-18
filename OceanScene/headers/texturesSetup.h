#ifndef TEXTURESSETUP_H
#define TEXTURESSETUP_H
// All textures used in the scene
extern unsigned int diffuseMap;
extern unsigned int specularMap;
extern unsigned int seaweedTex;
extern unsigned int fishTex;

// Loading texture function
unsigned int loadTexture(char const * path);
void texturesSetup();

#endif
