#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include "glm.hpp"
#include "terrain.h"
#include "seaweed.h"
#include "fileLoader.h"
#include "camera.h"
#include "rock.h"

#include <string>
#include <vector>
using namespace std;



class Scene {
public:

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    FileLoader fileSys;
    Terrain* terrain;
    Seaweed* seaweed;
    Rock* rock;
    int seed;

    Scene(int seed)
    {
        //terrain = new Terrain(10, 15., 150., .2, 0.1, .8);
        //seaweed = new Seaweed(100);
        // rock arguments: nr Vertices, length of cube grid, rockPosition, isosurface level, noiseScale, type of generation
        rock = new Rock(100.0f, 30.0f, glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, 0.1f, GEN_ROCK);
        seed = seed;

    }

    // render the mesh
    void Draw(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, Camera camera, float time)
    {
      // Clear screen
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view/projection/model transformations
      projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
      view = camera.GetViewMatrix();
      model = glm::mat4(1.0f);


      // Draw all the things in the scene
      //terrain->Draw(model, view, projection, time, camera.Position);
      //seaweed->Draw(model, view, projection, time);
      rock->Draw(model, view, projection, camera.Position);



    }
};
#endif
