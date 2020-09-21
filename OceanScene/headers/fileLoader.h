
#include <string>
#include <vector>
#include "json.hpp"
#include <fstream>

using namespace std;


class FileLoader {
public:

    // constructor
    FileLoader()
    {

    }

    // render the mesh
    void WriteFile()
    {
      nlohmann::json j;
      j["chunks"] = 6;
      j["chunkSize"] = 100;
      j["data"] = {1, 2, 3, 4, 5, 6};

      std::ofstream myFile;
      myFile.open("../saveFiles/test.json");
      myFile << j;
      myFile.close();


    }


private:

};
