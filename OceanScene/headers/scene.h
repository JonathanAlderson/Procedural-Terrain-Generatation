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
#include "beizer.h"
#include "rocks.h"

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
    Rocks* rocks;

    int seed;
    float waterSize;

    int movingCamera;

    // Camera Movement
    std::vector<glm::vec3> cameraMovePoints = {glm::vec3(90.0369, -6.62232, 101.567),
                                               glm::vec3(-99.9266, 50.03357, 74.9608),
                                               glm::vec3(-85.7807, -8.02192, -93.2215),
                                               };

    std::vector<glm::vec3> cameraRotatePoints = {glm::vec3(-180.4, 7.2, 0.),
                                                 glm::vec3(-0.899945, -1.5, 0.),
                                                 glm::vec3(116.1, -6, 0.),
                                                 };

    BCurve * cameraMoveSpline;
    BCurve * cameraRotateSpline;
    float moveSpeed = 0.04;

    Scene(WaterFrameBuffers *fbos, Camera *camera, int movingCam)
    {
        this->movingCamera = movingCam;
        // Set Seed
        srand(time(NULL));
        seed = rand()%10000;

        // Known Seeds

        // large central island 8647
        seed = 8647;

        // Settings
        int maxSeaweed = 4000;
        int maxFish = 100;
        int schoolSize = 20;

        // Terrain
        std::cout << "Seed: " << seed << '\n';
        terrain = new Terrain(seed, 20, 1., 50., .8, .25, .25, 70., maxSeaweed);
        maxSeaweed = terrain->maxSeaweed;

        // Seaweed
        std::cout << "Max Seaweed: " << maxSeaweed << '\n';
        seaweed = new Seaweed(maxSeaweed, terrain->seaweedPos);

        // Water
        waterTile = new WaterTile(0., 0., 0.);
        water = new Water(fbos);
        waterSize = .5 * terrain->numChunks * terrain->chunkSize * terrain->scale;

        // Rocks

        std::vector<glm::vec3> cameraRotatePoints = {glm::vec3(-180.4, 7.2, 0.),
                                                     glm::vec3(-0.899945, -1.5, 0.),
                                                     glm::vec3(116.1, -6, 0.),
                                                     };



        std::vector<glm::vec3> rockLocs = {glm::vec3(0., 0., 0.),
                                          glm::vec3(0., 0., 20.),
                                          glm::vec3(0., 0., 40.),
                                          glm::vec3(20., 0., 0.),
                                          glm::vec3(20., 0., 20.),
                                          glm::vec3(20., 0., 40.),
                                          glm::vec3(40., 0., 0.),
                                          glm::vec3(40., 0., 20.),
                                          glm::vec3(40., 0., 40.)};

        rocks = new Rocks(10.0f, 30.0f, rockLocs, 0.5f, 0.1f, GEN_ROCK);

        std::cout << "ROCKS SETUP" << '\n';
        // Skybox
        skybox = new Skybox("skybox1");

        // Fish
        fish = new Fish(maxFish, schoolSize, waterSize);

        // Camera Movement
        // Beizer Curve Stuff
        cameraMoveSpline = new BCurve(cameraMovePoints, 100);
        cameraRotateSpline = new BCurve(cameraRotatePoints, 100);

        std::cout << "Made Splines " << '\n';


    }

    void MoveCamera(Camera* camera, float t)
    {
      if(movingCamera == 1)
      {
        if(cameraMovePoints.size() > 1)
        {
          camera->Position = cameraMoveSpline->get(t * moveSpeed);
        }
        if(cameraRotatePoints.size() > 1)
        {
          camera->Yaw = cameraRotateSpline->get(t * moveSpeed).x;
          camera->Pitch = cameraRotateSpline->get(t * moveSpeed).y;
        }
        camera->updateCameraVectors();
      }
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
      //terrain->Draw(this->model, this->view, this->projection, time, camera.Position, clipPlane);
      //seaweed->Draw(this->model, this->view, this->projection, time, clipPlane);
      //water->Draw(waterTile, this->model, this->view, this->projection, time, camera, clipPlane, waterSize);
      //fish->Draw(this->model, this->view, this->projection, time, clipPlane);
      rocks->Draw(model, view, projection, camera.Position, clipPlane);

      //rock2->Draw(model, view, projection, camera.Position, clipPlane);
      //rock3->Draw(model, view, projection, camera.Position, clipPlane);
      //rock4->Draw(model, view, projection, camera.Position, clipPlane);
      skybox->Draw(this->projection, camera, clipPlane);
    }

    // render everything
    void DrawNoWater(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, Camera camera, float time, glm::vec4 clipPlane)
    {
      DrawSetup(SCR_WIDTH, SCR_HEIGHT, camera);
      // Draw all the things in the scene
      //terrain->Draw(this->model, this->view, this->projection, time, camera.Position, clipPlane);
      //seaweed->Draw(this->model, this->view, this->projection, time, clipPlane);
      //fish->Draw(this->model, this->view, this->projection, time, clipPlane);
      rocks->Draw(model, view, projection, camera.Position, clipPlane);
      //rock2->Draw(model, view, projection, camera.Position, clipPlane);
      //rock3->Draw(model, view, projection, camera.Position, clipPlane);
      //rock4->Draw(model, view, projection, camera.Position, clipPlane);
      skybox->Draw(this->projection, camera, clipPlane);
    }


};
#endif
