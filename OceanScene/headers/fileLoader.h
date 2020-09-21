#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include <vector>
#include "json.hpp"
#include "terrain.h"
#include <fstream>
#include <iomanip>

using namespace std;


class FileLoader {
public:

    // constructor
    FileLoader()
    {

    }

    // Check if this map has been generated before
    void fileExists(int seed)
    {

    }

    // render the mesh
    void writeTerrainFile(int seed, Terrain* t)
    {
      nlohmann::json j;

      vector<int> indicesSave;
      vector<float> positions;
      vector<float> normals;


      // Get the indicies
      for(int i = 0; i < t->indicesSize; i++)
      {
        indicesSave.push_back(t->chunks[0].indices[i]);
      }

      for(int i = 0; i < t->numChunks * t->numChunks; i++)
      {
        for(int j = 0; j < ((t->chunkSize)+1)*((t->chunkSize)+1); j++)
        {
          //std::cout << i << " " << j << std::endl;
          positions.push_back(t->vertices[i][j].Position.x);
          positions.push_back(t->vertices[i][j].Position.y);
          positions.push_back(t->vertices[i][j].Position.z);

          normals.push_back(t->vertices[i][j].Normal.x);
          normals.push_back(t->vertices[i][j].Normal.y);
          normals.push_back(t->vertices[i][j].Normal.z);
        }
      }

      // Put data in JSON
      j["numChunks"] = t->numChunks;
      j["chunkSize"] = t->chunkSize;
      j["indiciesSize"] = t->indicesSize;
      j["indices"] = indicesSave;
      j["positions"] = positions;
      j["normals"] = normals;


      std::ofstream myFile;
      myFile.open("../saveFiles/" + to_string(seed) + ".json");
      myFile << std::setw(4) << j << std::endl;
      myFile.close();

      std::cout << " writing to file" << std::endl;


    }

private:

};
#endif
