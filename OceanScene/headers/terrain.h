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
    glm::vec3 *positions;
    glm::vec3 *normals;
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
        positions = (glm::vec3 *) malloc(verticesSize * sizeof(glm::vec3));
        normals = (glm::vec3 *) calloc(verticesSize, sizeof(glm::vec3));
        indices = (int *) malloc(indicesSize * sizeof(int));

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        generatePoints();
        setupMesh();

        // Being good
        //free(vertices);
        //free(indices);
        //free(normals);
        //free(positions)
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

      int count;
      count = 0;

      // normals
      glm::vec3 lowerNormal;
      glm::vec3 upperNormal;
      glm::vec3 vert1;
      glm::vec3 vert2;
      glm::vec3 vert3;
      glm::vec3 vert4;


      for(int z = 0; z < zRange; z++)
      {
        for(int x = 0; x < xRange; x++)
        {
          float height = glm::perlin(glm::vec2((float)x/noiseScale, (float)z/noiseScale)) * heightScale;

          vertX = x * scale;
          vertZ = z * scale;

          // Update this Vertexs' positon
          positions[(xRange * z) + x] = glm::vec3(vertX, height, vertZ);

          //std::cout << (xRange * z + x) * 3 << ": " << vertX << " - " << vertZ << std::endl;

          // If we are not on bottom or rightmost edge
          if(x < xRange - 1 && z < zRange - 1)
          {
            // Find the bottom right square of
            // indicies to create mesh
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

          // If we are not on the top or leftmost edge
          if(z > 0 && x > 0)
          {
            // Find the top left square
            // of indicies to create normals
            ind4 = x + (z * xRange);
            ind3 = x-1 + (z * xRange);
            ind2 = x + ((z-1) * xRange);
            ind1 = x-1 + ((z-1) * xRange);

            vert1 = positions[ind1];
            vert2 = positions[ind2];
            vert3 = positions[ind3];
            vert4 = positions[ind4];

            // Calc normals for two planes
            lowerNormal = normal(vert1, vert4, vert3);
            upperNormal = normal(vert1, vert2, vert4);

            // Add normals
            normals[ind1] += lowerNormal + upperNormal;
            normals[ind2] += upperNormal;
            normals[ind3] += lowerNormal;
            normals[ind4] += lowerNormal + upperNormal;
          }
        }
      }

      // Put the positions and normals into the vertex struct
      Vertex thisVertex;
      glm::vec3 thisNormal;
      for(int i = 0; i < verticesSize; i++)
      {
        thisNormal = glm::normalize(normals[i]);
        thisVertex.Position = positions[i];
        thisVertex.Normal =  thisNormal;
        vertices[i] = thisVertex;
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
