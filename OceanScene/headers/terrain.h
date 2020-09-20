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
    int verticesSizeBorder; // used for the extra ring for normal calculations
    int indicesSize;

    unsigned int VAO;
    int numChunks;
    float waterLevel;
    float landPrevelence;
    float heightMultiplier;
    int heightScale;
    int noiseScale;
    float scale;

    // constructor
    Terrain(int numChunks, float heightScale, float noiseScale, float scale, float waterLevel, float landPrevelence)
    {
        this->numChunks = numChunks;
        this->heightScale = heightScale;
        this->noiseScale = noiseScale;
        this->scale = scale;
        this->waterLevel = waterLevel;
        this->landPrevelence = landPrevelence;
        this->heightMultiplier = ((chunkSize * numChunks)/2.) * landPrevelence;

        // Malloc for each chunk
        verticesSize = (chunkSize+1) * (chunkSize+1);
        verticesSizeBorder = (chunkSize+3) * (chunkSize+3);
        indicesSize = (chunkSize) * (chunkSize) * 6;
        vertices = (Vertex *) malloc(verticesSize * sizeof(Vertex));
        positions = (glm::vec3 *) malloc(verticesSizeBorder * sizeof(glm::vec3));
        normals = (glm::vec3 *) calloc(verticesSizeBorder, sizeof(glm::vec3));
        indices = (int *) malloc(indicesSize * sizeof(int));

        // Malloc for all the chunks
        chunks = (Chunk *) malloc(numChunks * numChunks * sizeof(Chunk));

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        for(int i = 0; i < numChunks; i++)
        {
          for(int j = 0; j < numChunks; j++)
          {
            // Reseting normals
            for(int k = 0; k < verticesSizeBorder; k++)
            {
              normals[k] = glm::vec3(0., 0., 0.);
            }
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
    int octaves = 1.;
    float ns = noiseScale;
    float hs = heightScale;

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
      //
      // std::cout << "Chunk: " << xChunk << ":" << zChunk << std::endl;

      // perlin noise settings
      float height;

      // -1 and +1 to include extra ring for normals calculations
      for(int z = -1; z <= chunkSize + 1; z++)
      {
        for(int x = -1; x <= chunkSize + 1; x++)
        {
          // Adjust positions based on chunks
          // Goes from chunk space to world space
          // Pos is used for position and sampling data
          // but not for index data
          posX = x + offsetX;
          posZ = z + offsetZ;

          vertX = posX * scale;
          vertZ = posZ * scale;

          // Calculate the height for this position
          height = getHeight(posX, posZ);

          // Update this Vertexs' positon
          positions[((chunkSize+3) * (z+1)) + (x+1)] = glm::vec3(vertX, height, vertZ);

          //std::cout << x << ":" << z << " --> " << ((chunkSize+3) * (z+1)) + (x+1) << std::endl;

          // If we are not on bottom or rightmost edge
          // or in the outer ring
          if(x > -1 && z > -1 && x <= chunkSize -1 && z <= chunkSize -1 )
          {
            // Find the bottom right square of
            // indicies to create mesh
            ind1 = x + (z * (chunkSize+1));
            ind2 = x + (z * (chunkSize+1)) + 1;
            ind3 = x + ((z+1) * (chunkSize+1));
            ind4 = x + ((z+1) * (chunkSize+1)) + 1;

            //std::cout << " Make Face " << ind1 << " " << ind2 << " " << ind3 << " " << ind4 << std::endl;
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
          if(z > -1 && x > -1)
          {
            // Find the top left square
            // of indicies to create normals

            ind4 = x+1 + ((z+1) * (chunkSize+3));
            ind3 = x + ((z+1) * (chunkSize+3));
            ind2 = x+1 + (z * (chunkSize+3));
            ind1 = x + (z * (chunkSize+3));


            // std::cout << " Normals " << ind1 << " " << ind2 << " " << ind3 << " " << ind4 << std::endl;

            vert1 = positions[ind1];
            vert2 = positions[ind2];
            vert3 = positions[ind3];
            vert4 = positions[ind4];



            // Calc normals for two planes
            lowerNormal = normal(vert1, vert4, vert3);
            upperNormal = normal(vert1, vert2, vert4);


            //std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            //std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;

            // Add normals
            normals[ind1] += lowerNormal + upperNormal;
            normals[ind2] += upperNormal;
            normals[ind3] += lowerNormal;
            normals[ind4] += lowerNormal + upperNormal;

            // if(ind1 == 25 || ind2 == 25 || ind3 == 25 || ind4 == 25)
            // {
            //   std::cout << "Currently -->  ";
            //   std::cout << normals[25].x << " " << normals[25].y << normals[25].z << std::endl;
            // }

            // bottom left
            // if((x == 0 && z == 0))
            // {
            //   //std::cout << "here " << ind1 << std::endl;
            //   std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            //   std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;
            // }
            // if((x == 1 && z == 0))
            // {
            //   std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            // }
            // if((x == 0 && z == 1))
            // {
            //   std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;
            // }
            // if((x == 1 && z == 1))
            // {
            //   std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            //   std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;
            // }

            // top left
            // if((x == 0 && z == 3))
            // {
            //   std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            //   std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;
            // }
            // if((x == 1 && z == 3))
            // {
            //   std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            // }
            // if((x == 0 && z == 4))
            // {
            //   std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;
            // }
            // if((x == 1 && z == 4))
            // {
            //   std::cout << lowerNormal.x << " " << lowerNormal.y << " " << lowerNormal.z << std::endl;
            //   std::cout << upperNormal.x << " " << upperNormal.y << " " << upperNormal.z << std::endl;
            // }

          }
        }
      }


      // Put the positions and normals into the vertex struct
      // mess around with indicies to not include the outer ring
      Vertex thisVertex;
      int counter = 0;
      glm::vec3 thisNormal;
      for(int i = 0; i < verticesSizeBorder; i++)
      {
        // If not in the first row
        if(i > chunkSize + 2)
        {
          // If not in the first column
          if(i%(chunkSize + 3) != 0)
          {
            // If not in the last column
            if(i%(chunkSize + 3) != 5)
            {
              // If not in the last row
              if(i < (chunkSize+3)*(chunkSize+2))
              {
                //std::cout << i << std::endl;
                // -(chunkSize+4)  go back one row one column
                //thisNormal = glm::normalize(normals[i - (chunkSize+4)]);
                thisNormal = glm::normalize(normals[i]);
                thisVertex.Position = positions[i - (chunkSize+4)];
                thisVertex.Normal =  thisNormal;
                vertices[counter] = thisVertex;

                // if(counter == 12)
                // {
                //   //std::cout << "Final Result:  ";
                //   //std::cout << i << std::endl;
                //   thisNormal = normals[i];
                //   std::cout << thisNormal.x << " " << thisNormal.y << " " << thisNormal.z << std::endl;
                //   thisNormal = glm::normalize(normals[i]);
                //   std::cout << thisNormal.x << " " << thisNormal.y << " " << thisNormal.z << std::endl;
                //
                // }

                //std::cout << "C: " << counter << " i: " << i << " --> " << i - (chunkSize+4) << std::endl;
                counter++;
              }
            }
          }
        }

        //std::cout << positions[i].x << ":" << positions[i].z << "  =  " << thisNormal.x << ", " << thisNormal.y << ", " << thisNormal.z << std::endl;
      }

    }


    float getHeight(float posX, float posZ)
    {
      // Get the height of the terrain
      // with different logic for being in the ocean
      // or on land

      float height = 0;
      float ns = noiseScale;
      float hs = heightScale;
      float toCenter = std::sqrt((posX * posX) + (posZ * posZ));
      float seaDepth;
      float seaOffset;

      // Mountains
      octaves = 4;
      for(int i = 0; i < octaves; i++)
      {
        height += ((glm::perlin(glm::vec2((float)posX/ns , (float)posZ/ns))+.707)/1.414) * hs;

        ns *= .5;
        hs *= ((height/heightScale))/1.5;
      }

      // Less Pronounced as you get further away
      height = height * (1. - std::min((toCenter/heightMultiplier), (1.0f)));

      // If we are underwater
      if((height/heightScale) < waterLevel)
      {
        seaOffset = 0.;
        seaDepth = waterLevel - (height/heightScale);

        ns = noiseScale;
        hs = heightScale/5.;
        seaOffset -= ((glm::perlin(glm::vec2((float)posX/ns , (float)posZ/ns))+.707)/1.414) * hs;

        ns = noiseScale/32.;
        hs = heightScale/70.;
        seaOffset -= ((glm::perlin(glm::vec2((float)(posX*34.1)/ns , (float)posZ/ns))+.707)/1.414) * hs;

        height = height + seaOffset * seaDepth * 4.;
      }



      return height;

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
