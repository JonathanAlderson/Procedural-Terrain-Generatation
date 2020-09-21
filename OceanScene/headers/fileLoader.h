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

    // render the mesh
    void WriteTerrainFile(int seed, Terrain* t)
    {
      nlohmann::json j;

      vector<int> indiciesSave;

      // Get the indicies
      for(int i = 0; i < t->indicesSize; i++)
      {
        indiciesSave.push_back(t->chunks[0].indices[i]);
      }

      j["numChunks"] = t->numChunks;
      j["chunkSize"] = t->chunkSize;
      j["indiciesSize"] = t->indicesSize;
      j["indicies"] = indiciesSave;

      std::ofstream myFile;
      myFile.open("../saveFiles/test.json");
      myFile << std::setw(4) << j << std::endl;
      myFile.close();

      std::cout << " writing to file" << std::endl;


    }


private:

};
#endif
