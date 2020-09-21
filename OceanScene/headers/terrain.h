#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <time.h>
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
    int chunkSize = 10.;

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
    float seed;
    Shader* shader = new Shader("terrain.vs", "terrain.fs");

    // constructor

    // Num chunks, heightscale, noiseScale, scale, waterLevel, landPrevelence
    // 10, 15., 150., .2, 0.1, .8


    Terrain(int numChunks, float heightScale, float noiseScale, float scale, float waterLevel, float landPrevelence)
    {
        this->numChunks = numChunks;
        this->heightScale = heightScale;
        this->noiseScale = noiseScale;
        this->scale = scale;
        this->waterLevel = waterLevel;
        this->landPrevelence = landPrevelence;
        this->heightMultiplier = ((chunkSize * numChunks)/2.) * landPrevelence;
        srand(time(NULL));
        std::cout << rand() << std::endl;
        this->seed = rand();//(float)rand();
        //this->shader = new Shader("terrain.vs", "terrain.fs");
        setupShader();

        // Malloc for each chunk
        verticesSize = (chunkSize+1) * (chunkSize+1);
        verticesSizeBorder = (chunkSize+3) * (chunkSize+3);
        indicesSize = (chunkSize) * (chunkSize) * 6;
        vertices = (Vertex *) malloc(verticesSize * sizeof(Vertex));
        positions = (glm::vec3 *) malloc(verticesSizeBorder * sizeof(glm::vec3));
        normals = (glm::vec3 *) calloc(verticesSizeBorder, sizeof(glm::vec3));
        indices = (int *) malloc(indicesSize * sizeof(int));
        chunks = (Chunk *) malloc(numChunks * numChunks * sizeof(Chunk));

        // Generate data for each chunk

        std::cout << &chunks[0] << std::endl;
        // numChunks
        for(int i = 0; i < numChunks; i++)
        {
          for(int j = 0; j < numChunks; j++)
          {
            generateChunkPoints(j, i);
            setupChunk(j, i);
          }
        }

        for(int i = 0; i < 10; i++)
        {
          std::cout << chunks[0].points[i].Position.y << "     " << chunks[12].points[i].Position.y  << std::endl; 
        }

        // Being good
        // free(vertices);
        // free(indices);
        // free(normals);
        // free(positions);
    }

    // render the mesh
    void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time, glm::vec3 camPos)
    {
        shader->use();
        shader->setFloat("time", time);
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setMat4("model", model);
        shader->setVec3("viewPos", camPos);
        // draw each chunk individually
        for(int i = 0; i < numChunks * numChunks; i++)
        {
          glBindVertexArray(chunks[i].VAO);
          glDrawElements(GL_TRIANGLES, indicesSize , GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }
    }

    void setupShader()
    {
      shader->use();
      shader->setFloat("shininess", 32.0f);
      shader->setFloat("maxHeight", heightScale);

      // directional light
      shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      shader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
      shader->setVec3("dirLight.diffuse", 1.f, 1.f, 1.f);
      shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
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
      int count;
      float vertX;
      float vertZ;
      float height;
      count = 0;

      // The indicies for a single face
      int ind1;
      int ind2;
      int ind3;
      int ind4;

      // Normals for a single face
      glm::vec3 lowerNormal;
      glm::vec3 upperNormal;
      glm::vec3 vert1;
      glm::vec3 vert2;
      glm::vec3 vert3;
      glm::vec3 vert4;

      Vertex thisVertex;
      glm::vec3 thisNormal;

      // World Position
      int posX;
      int posZ;
      int offsetX =  - chunkSize * ((numChunks)/2 - xChunk);
      int offsetZ =  - chunkSize * ((numChunks)/2 - zChunk);


      // Reset normals
      for(int k = 0; k < verticesSizeBorder; k++)
      {
        normals[k] = glm::vec3(0., 0., 0.);
      }


      // Step through each vertex on and around chunk
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

          // Create Face
          if(x > -1 && z > -1 && x <= chunkSize -1 && z <= chunkSize -1 )
          {
            // Find the bottom right square of
            // indicies to create mesh
            ind1 = x + (z * (chunkSize+1));
            ind2 = x + (z * (chunkSize+1)) + 1;
            ind3 = x + ((z+1) * (chunkSize+1));
            ind4 = x + ((z+1) * (chunkSize+1)) + 1;

            // Assign correct indicies
            indices[count * 6    ] = ind1;
            indices[count * 6 + 1] = ind4;
            indices[count * 6 + 2] = ind3;
            indices[count * 6 + 3] = ind1;
            indices[count * 6 + 4] = ind2;
            indices[count * 6 + 5] = ind4;
            count ++;
          }

          // Calculate Plane Normals
          if(z > -1 && x > -1)
          {
            // Find the top left square
            // of indicies to create normals

            ind4 = x+1 + ((z+1) * (chunkSize+3));
            ind3 = x + ((z+1) * (chunkSize+3));
            ind2 = x+1 + (z * (chunkSize+3));
            ind1 = x + (z * (chunkSize+3));

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

      // Put Positions and Normals into Struct
      count = 0; // reset counter
      for(int i = 0; i < verticesSizeBorder; i++)
      {

        // If not in the first or last row
        if(i > chunkSize + 2 && i < (chunkSize+3)*(chunkSize+2))
        {
          // If not in the first or last column
          if(i%(chunkSize + 3) != 0 && i%(chunkSize + 3) != 5)
          {
            // Put data in struct
            thisNormal = glm::normalize(normals[i]);
            thisVertex.Position = positions[i - (chunkSize+4)];
            thisVertex.Normal =  thisNormal;
            vertices[count] = thisVertex;
            count++;
          }
        }
      }
    }

    glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
      return glm::cross(c-a, b-a);
    }

    float getHeight(float posX, float posZ)
    {
      // Get the height of the terrain
      float height = 0;
      float ns = noiseScale;
      float hs = heightScale;
      float toCenter = std::sqrt((posX * posX) + (posZ * posZ));
      float seaDepth;
      float seaOffset;
      octaves = 4;

      // Add the seed
      posX += seed;
      posZ += seed;

      // Mountains
      for(int i = 0; i < octaves; i++)
      {
        height += ((glm::perlin(glm::vec2((float)posX/ns , (float)posZ/ns))+.707)/1.414) * hs;
        ns *= .5;
        hs *= ((height/heightScale))/1.5;
      }

      // Less Pronounced as you get further away
      height = height * 1. / max((6. * (toCenter/heightMultiplier)), 1.);

      // If we are underwater
      if((height/heightScale) < waterLevel)
      {
        // To make a smooth transition
        seaOffset = 0.;
        seaDepth = waterLevel - (height/heightScale);

        // Sand Dunes
        ns = noiseScale;
        hs = heightScale/5.;
        seaOffset -= ((glm::perlin(glm::vec2((float)posX/ns , (float)posZ/ns))+.707)/1.414) * hs;

        // Actual Sand
        ns = noiseScale/32.;
        hs = heightScale/70.;
        seaOffset -= ((glm::perlin(glm::vec2((float)(posX*34.1)/ns , (float)posZ/ns))+.707)/1.414) * hs;

        height = height + seaOffset * seaDepth * 4.;
      }
      height -= waterLevel * heightScale;

      return height;
    }

    // initializes all the buffer objects/arrays
    // inside the chunk from the current data
    void setupChunk(int x, int z)
    {
        // Create the chunk and put it in our chunks
        int chunkID = x + (z*numChunks);
        Chunk thisChunk = {VAO : 0,
                           VBO : 0,
                           EBO : 0,
                           points : vertices,
                           indices : indices,
                           x : x,
                           z : z,
                           id : chunkID,
                           chunkSize : chunkSize};

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

        // VERTEX ATTRIBUTE POINTERS
        // vertex Positions and vertex normals
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glBindVertexArray(0);

        // Put the data in the main chunks
        chunks[chunkID] = thisChunk;
    }
};
#endif
