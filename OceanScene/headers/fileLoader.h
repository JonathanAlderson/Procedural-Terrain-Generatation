#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include <vector>
#include "json.hpp"
#include "terrain.h"
#include <fstream>
#include <iomanip>




using json = nlohmann::json;
namespace glm
{
    void to_json(json& j, const glm::vec3& P)
    {
        j = { { "x", P.x }, { "y", P.y }, {"z", P.z} };
    };

    void from_json(const json& j, glm::vec3& P) {
        j.at("x").get_to(P.x);
        j.at("y").get_to(P.y);
        j.at("z").get_to(P.z);
    }
}

using json = nlohmann::json;
namespace v
{
  void to_json(json& j, const Vertex& v)
  {
    j = json{{"Position", v.Position}, {"Normal", v.Normal}};
  }

  void from_json(const json& j, Vertex& v) {
    j.at("Position").get_to<glm::vec3>(v.Position);
    j.at("Normal").get_to<glm::vec3>(v.Normal);
  }
}


using namespace std;
class FileLoader {
public:

    // constructor
    FileLoader()
    {

    }

    // Check if this map has been generated before
    bool fileExists(int seed)
    {
      if (FILE *file = fopen(("../saveFiles/" + to_string(seed) + ".json").c_str(), "r")) {
          fclose(file);
          std::cout << "File Exists" << std::endl;
          return true;
      } else {
          std::cout << "File Not Exists" << '\n';
          return false;
      }
    }

    // Read The Mesh
    nlohmann::json readTerrainFile(int seed)
    {
      std::cout << "Reading file" << '\n';
      // Read in the file to J
      nlohmann::json j;
      std::ifstream i("../saveFiles/" + to_string(seed) + ".json");
      i >> j;
      return j;
    }

    // Save The Mesh
    void writeTerrainFile(int seed, Terrain* t)
    {
      nlohmann::json j;

      // Put data into vectors to save
      vector<int> indicesSave;
      vector<v::Vertex> pointsSave;
      vector<vector<v::Vertex>> allPoints;


      std::cout << "NumChunks: " << t->numChunks * t->numChunks << '\n';
      std::cout << "Verts: " << t->verticesSize << '\n';
      std::cout << "Vertex: " << sizeof(Vertex) << '\n';

      for(int i = 0; i < t->indicesSize; i++)
      {
        indicesSave.push_back(t->chunks[0].indices[i]);
      }

      for(int j = 0; j < (t->numChunks * t->numChunks); j++)
      {
        for(int i = 0; i < t->verticesSize; i++)
        {
          pointsSave.push_back(t->chunks[j].points[i]);
        }
        allPoints.push_back(pointsSave);
        pointsSave.clear();
      }

      // Put data in JSON
      j["seed"] = t->seed;
      j["numChunks"] = t->numChunks;
      j["chunkSize"] = t->chunkSize;
      j["indicesSize"] = t->indicesSize;
      j["heightScale"] = t->heightScale;
      j["indices"] = indicesSave;
      j["points"] = allPoints;

      std::ofstream myFile;
      myFile.open("../saveFiles/" + to_string(seed) + ".json");
      myFile << std::setw(4) << j << std::endl;
      myFile.close();

      std::cout << "Saving To " << to_string(seed) << ".json" << std::endl;


    }

private:

};


#endif
