#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include "glm.hpp"
#include "terrain.h"
#include "seaweed.h"
#include "fileLoader.h"
#include "skybox.h"
#include "camera.h"
#include "json.hpp"
#include "water.h"
#include "fish.h"
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
    Skybox* skybox;
    Fish* fish;

    int seed;
    float waterSize;

    Scene(WaterFrameBuffers *fbos)
    {
        srand(time(NULL));
        seed = rand()%10000;

        // Known Seeds

        // large central island 8647
        seed = 8647;

        int maxSeaweed = 20000;
        int maxFish = 3000;
        int schoolSize = 20;

        // Terrain
        std::cout << "Seed: " << seed << '\n';
        terrain = new Terrain(seed, 30, 15., 50., .8, 0.1, .1, 30., maxSeaweed);
        maxSeaweed = terrain->maxSeaweed;

        // Seaweed
        std::cout << "Max Seaweed: " << maxSeaweed << '\n';
        seaweed = new Seaweed(maxSeaweed, terrain->seaweedPos);

        // Water
        waterTile = new WaterTile(0., 0., 0.);
        water = new Water(fbos);
        waterSize = .5 * terrain->numChunks * terrain->chunkSize * terrain->scale;

        // Skybox
        skybox = new Skybox("skybox1");

        // Fish
        fish = new Fish(maxFish, schoolSize, waterSize);


    }

    void DrawSetup(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, Camera camera)
    {
      // Does everything to draw except the actual drawing
      // Clear screen
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view/projection/model transformations
      this->projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
      this->view = camera.GetViewMatrix();
      this->model = glm::mat4(1.0f);
    }

    // render everything
    void Draw(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, Camera camera, float time, glm::vec4 clipPlane)
    {

      DrawSetup(SCR_WIDTH, SCR_HEIGHT, camera);
      // Draw all the things in the scene
      terrain->Draw(this->model, this->view, this->projection, time, camera.Position, clipPlane);
      seaweed->Draw(this->model, this->view, this->projection, time, clipPlane);
      water->Draw(waterTile, this->model, this->view, this->projection, time, camera, clipPlane, waterSize);
      fish->Draw(this->model, this->view, this->projection, time, clipPlane);
      skybox->Draw(this->projection, camera, clipPlane);
    }

    // render everything
    void DrawNoWater(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, Camera camera, float time, glm::vec4 clipPlane)
    {
      DrawSetup(SCR_WIDTH, SCR_HEIGHT, camera);
      // Draw all the things in the scene
      terrain->Draw(this->model, this->view, this->projection, time, camera.Position, clipPlane);
      seaweed->Draw(this->model, this->view, this->projection, time, clipPlane);
      fish->Draw(this->model, this->view, this->projection, time, clipPlane);
      skybox->Draw(this->projection, camera, clipPlane);
    }


};
#endif
