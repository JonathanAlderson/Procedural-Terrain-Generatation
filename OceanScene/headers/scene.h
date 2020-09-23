#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include "glm.hpp"
#include "terrain.h"
#include "seaweed.h"
#include "fileLoader.h"
#include "camera.h"
#include "json.hpp"
#include "water.h"
#include "waterTile.h"
#include <time.h>

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
    Water* water;
    WaterTile* waterTile;

    int seed;

    Scene()
    {
        srand(time(NULL));
        seed = rand()%10000;
        int maxSeaweed = 20000;
        std::cout << "Seed: " << seed << '\n';

        terrain = new Terrain(seed, 6, 15., 150., .2, 0.1, 2., 30., maxSeaweed);
        maxSeaweed = terrain->maxSeaweed;

        std::cout << "Max Seaweed: " << maxSeaweed << '\n';
        seaweed = new Seaweed(maxSeaweed, terrain->seaweedPos);


        waterTile = new WaterTile(0., 0., 0.);
        water = new Water();




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
      terrain->Draw(model, view, projection, time, camera.Position);
      seaweed->Draw(model, view, projection, time);
      water->Draw(waterTile, model, view, projection, camera);



    }
};
#endif
