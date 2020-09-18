#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
};

class Terrain {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;

    unsigned int VAO;
    int xRange;
    int zRange;
    int heightScale;
    int noiseScale;
    int scale;

    // constructor
    Terrain(int xRange, int zRange, float heightScale, float noiseScale, float scale)
    {
        this->xRange = xRange;
        this->zRange = zRange;
        this-> heightScale = heightScale;
        this-> noiseScale = noiseScale;
        this ->scale = scale;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        generatePoints();
        setupMesh();
    }

    // render the mesh
    void Draw()
    {
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


private:
    // render data
    unsigned int VBO, EBO;


    // creates the verticies and the indicies and the normals
    void generatePoints()
    {
      // Current Position
      float vertX;
      float vertZ;

      // The indicies for a single face
      int ind1;
      int ind2;
      int ind3;
      int ind4;

      Vertex thisVertex;
      int count;
      count = 0;


      for(int z = 0; z < zRange; z++)
      {
        for(int x = 0; x < xRange; x++)
        {
          float height = glm::perlin(glm::vec2((float)x/noiseScale, (float)z/noiseScale)) * heightScale;

          vertX = x * scale;
          vertZ = z * scale;

          thisVertex = {glm::vec3(vertX, height, vertZ), glm::vec3(0., 0., 0.) };

          vertices.push_back(thisVertex);

          //std::cout << (xRange * z + x) * 3 << ": " << vertX << " - " << vertZ << std::endl;

          // Only make the indicies if we are not on the edge
          if(x < xRange - 1 && z < zRange - 1)
          {
            ind1 = x + (z * xRange);
            ind2 = x + (z * xRange) + 1;
            ind3 = x + ((z+1) * xRange);
            ind4 = x + ((z+1) * xRange) + 1;

            // Assign correct indicies
            indices.push_back(ind1);
            indices.push_back(ind4);
            indices.push_back(ind3);
            indices.push_back(ind1);
            indices.push_back(ind2);
            indices.push_back(ind4);
            count ++;
          }
        }
      }
    }

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }
};
#endif
