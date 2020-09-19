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


struct Chunk {
  unsigned int VAO, VBO, EBO;
  Vertex *points;
  int *indices;
  int x;
  int z;
  int id;
  int chunkSize;
};

class Terrain {
public:

    // Terrain made out of chunks of fixed size
    int chunkSize = 100.;

    Chunk *chunks;

    // mesh Data
    Vertex *vertices;
    glm::vec3 *positions;
    glm::vec3 *normals;
    int *indices;

    int verticesSize;
    int indicesSize;

    unsigned int VAO;
    int numChunks;
    float waterLevel;
    int heightScale;
    int noiseScale;
    float scale;

    // constructor
    Terrain(int numChunks, float heightScale, float noiseScale, float scale, float waterLevel)
    {
        this->numChunks = numChunks;
        this->heightScale = heightScale;
        this->noiseScale = noiseScale;
        this->scale = scale;
        this->waterLevel = waterLevel;

        // Malloc for each chunk
        verticesSize = chunkSize * chunkSize;
        indicesSize = (chunkSize - 1) * (chunkSize - 1) * 6;
        vertices = (Vertex *) malloc(verticesSize * sizeof(Vertex));
        positions = (glm::vec3 *) malloc(verticesSize * sizeof(glm::vec3));
        normals = (glm::vec3 *) calloc(verticesSize, sizeof(glm::vec3));
        indices = (int *) malloc(indicesSize * sizeof(int));

        // Malloc for all the chunks
        chunks = (Chunk *) malloc(numChunks * numChunks * sizeof(Chunk));

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        for(int i = 0; i < numChunks; i++)
        {
          for(int j = 0; j < numChunks; j++)
          {
            generateChunkPoints(j, i);
            setupChunk(j, i);
          }
        }

        // Being good
        free(vertices);
        free(indices);
        free(normals);
        free(positions);
    }

    // render the mesh
    void Draw()
    {
        // draw mesh
        // draw each chunk individually
        for(int i = 0; i < numChunks * numChunks; i++)
        {
          glBindVertexArray(chunks[i].VAO);
          glDrawElements(GL_TRIANGLES, indicesSize , GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }

    }


private:

    // creates the verticies and the indicies and the normals
    // puts all current chunk information inside vertices, normals, indices
    void generateChunkPoints(int xChunk, int zChunk)
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

      int posX;
      int posZ;
      int offsetX =  - chunkSize * ((numChunks)/2 - xChunk);
      int offsetZ =  - chunkSize * ((numChunks)/2 - zChunk);

      //std::cout << xChunk << " : " << zChunk << " " << offsetX << "  =  " << offsetZ << std::endl;

      // perlin noise settings
      int octaves = 1.;

      float height;

      float ns = noiseScale;
      float hs = heightScale;

      for(int z = 0; z < chunkSize; z++)
      {
        for(int x = 0; x < chunkSize; x++)
        {
          // Adjust positions based on chunks
          // Goes from chunk space to world space
          // Pos is used for position and sampling data
          // but not for index data
          posX = x + offsetX;
          posZ = z + offsetZ;


          // reset the height
          height = 0;
          ns = noiseScale;
          hs = heightScale;

          for(int i = 0; i < octaves; i++)
          {
            height += glm::perlin(glm::vec2((float)posX/ns , (float)posZ/ns)) * hs;
            ns *= .5;
            hs *= (float)z/chunkSize;
          }

          vertX = posX * scale;
          vertZ = posZ * scale;

          // Update this Vertexs' positon
          positions[(chunkSize * z) + x] = glm::vec3(vertX, height, vertZ);

          //std::cout << x << ":" << z << "  =  " << height << std::endl;

          // If we are not on bottom or rightmost edge
          if(x < chunkSize - 1 && z < chunkSize - 1)
          {
            // Find the bottom right square of
            // indicies to create mesh
            ind1 = x + (z * chunkSize);
            ind2 = x + (z * chunkSize) + 1;
            ind3 = x + ((z+1) * chunkSize);
            ind4 = x + ((z+1) * chunkSize) + 1;

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
            ind4 = x + (z * chunkSize);
            ind3 = x-1 + (z * chunkSize);
            ind2 = x + ((z-1) * chunkSize);
            ind1 = x-1 + ((z-1) * chunkSize);

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
        //std::cout << positions[i].x << ":" << positions[i].z << "  =  " << thisNormal.x << ", " << thisNormal.y << ", " << thisNormal.z << std::endl;
      }

    }

    glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
      return glm::cross(c-a, b-a);
    }

    // initializes all the buffer objects/arrays
    // inside the chunk from the current data
    void setupChunk(int x, int z)
    {
        // Create the chunk and put it in our chunks
        int chunkID = x + (z*numChunks);
        //VAO, VBO, EBO
        Chunk thisChunk = {.points = vertices,
                           .indices = indices,
                           .x = x,
                           .z = z,
                           .id = chunkID,
                           .chunkSize = chunkSize};

        // create buffers/arrays
        glGenVertexArrays(1, &thisChunk.VAO);
        glGenBuffers(1, &thisChunk.VBO);
        glGenBuffers(1, &thisChunk.EBO);

        glBindVertexArray(thisChunk.VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, thisChunk.VBO);
        glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thisChunk.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);

        // Put the data in the main chunks
        chunks[chunkID] = thisChunk;
    }
};
#endif
