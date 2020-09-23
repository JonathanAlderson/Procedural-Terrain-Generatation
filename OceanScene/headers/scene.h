#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include "glm.hpp"
#include "terrain.h"
#include "seaweed.h"
#include "fileLoader.h"
#include "camera.h"
#include "json.hpp"
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
    int seed;

    Scene()
    {
        srand(time(NULL));
        seed = rand()%10000;

        int maxSeaweed = 1000;

        //seed = 1;
        std::cout << "Seed: " << seed << '\n';
        fileSys = FileLoader();

        // if(fileSys.fileExists(seed))
        // {
        //   nlohmann::json j = fileSys.readTerrainFile(seed);
        //   //auto json = nlohmann::json::parse(j);
        //
        //   terrain = new Terrain(j["seed"].get<int>(),
        //                         j["numChunks"].get<int>(),
        //                         j["heightScale"].get<int>(),
        //                         j["indicesSize"].get<int>(),
        //                         j["indices"].get<vector<int>>(),
        //                         j["points"].get<vector<vector<v::Vertex>>>());
        // }
        // else
        // {
          terrain = new Terrain(seed, 6, 15., 150., .2, 0.1, 2., 30., maxSeaweed);


        //   fileSys.writeTerrainFile(seed, terrain);
        // }

        //
        seaweed = new Seaweed(maxSeaweed, terrain->seaweedPos);

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



    }
};
#endif
