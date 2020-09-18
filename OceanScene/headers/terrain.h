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

    Vertex *vertices;
    int *indices;

    int verticesSize;
    int indicesSize;

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

        // Malloc
        verticesSize = xRange * zRange;
        indicesSize = (xRange - 1) * (zRange - 1) * 6;
        vertices = (Vertex *) malloc(verticesSize * sizeof(Vertex));
        indices = (int *) malloc(indicesSize * sizeof(int));

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        generatePoints();
        setupMesh();

        // Being good
        //free(vertices);
        //free(indices);
    }

    // render the mesh
    void Draw()
    {
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indicesSize , GL_UNSIGNED_INT, 0);
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

          vertices[(xRange * z) + x] = thisVertex;

          //std::cout << (xRange * z + x) * 3 << ": " << vertX << " - " << vertZ << std::endl;

          // Only make the indicies if we are not on the edge
          if(x < xRange - 1 && z < zRange - 1)
          {
            ind1 = x + (z * xRange);
            ind2 = x + (z * xRange) + 1;
            ind3 = x + ((z+1) * xRange);
            ind4 = x + ((z+1) * xRange) + 1;

            // Assign correct indicies
            indices[count * 6    ] = ind1;
            indices[count * 6 + 1] = ind4;
            indices[count * 6 + 2] = ind3;
            indices[count * 6 + 3] = ind1;
            indices[count * 6 + 4] = ind2;
            indices[count * 6 + 5] = ind4;

            count ++;
          }
        }
      }
    }

    glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
      return glm::cross(b-a, c-a);
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
        glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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
