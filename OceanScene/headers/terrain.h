#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <time.h>
#include "glm.hpp"
#include "json.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"

#include "shader.h"

#include <string>
#include <vector>


namespace v
{
  struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;

  };
}

using namespace std;
struct Chunk {
  unsigned int VAO, VBO, EBO;
  v::Vertex *points;
  int *indices;
  int x;
  int z;
  int id;
  int chunkSize;
};

using namespace v;
class Terrain {
public:

    // Terrain made out of chunks of fixed size
    int chunkSize = 100.;

    Chunk *chunks;

    // mesh Data
    Vertex **vertices;
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
    int vertRowLen; // For memory offsets
    int chunkID;
    float roughness;

    // for generating seaweeds
    glm::vec3* seaweedPos;
    int maxSeaweed;
    int seaweedCount;
    float seaweedMin;
    float maxSeaweedSpawnChance = 0.15;
    int didntSpawn = 0;
    int didSpawn = 0;

    // constructor

    // Num chunks, heightscale, noiseScale, scale, waterLevel, landPrevelence
    // 10, 15., 150., .2, 0.1, .8

    // Constructor For If We Want To Generate A New Map
    Terrain(int seed, int numChunks, float heightScale, float noiseScale, float scale, float waterLevel, float landPrevelence, float roughness, int maxSeaweed)
    {
        std::cout << "Generating New Map" << '\n';
        this->numChunks = numChunks;
        this->heightScale = heightScale;
        this->noiseScale = noiseScale;
        this->scale = scale;
        this->roughness = roughness;
        this->waterLevel = waterLevel;
        this->landPrevelence = landPrevelence;
        this->seed = seed;//(float)rand();

        // Seaweed Things
        this->maxSeaweed = maxSeaweed;
        this->seaweedCount = 0;


        seaweedSetup();
        //this->shader = new Shader("terrain.vs", "terrain.fs");
        setupShader();

        // Malloc for each chunk
        verticesSize = (chunkSize+1) * (chunkSize+1);
        verticesSizeBorder = (chunkSize+3) * (chunkSize+3);
        indicesSize = (chunkSize) * (chunkSize) * 6;

        vertices = (Vertex **) malloc(numChunks * numChunks * sizeof(Vertex *));
        for(int i = 0; i < numChunks * numChunks; i++)
        {
          vertices[i] = (Vertex *) malloc(verticesSize * sizeof(Vertex));
        }
        positions = (glm::vec3 *) malloc(verticesSizeBorder * sizeof(glm::vec3));
        normals = (glm::vec3 *) calloc(verticesSizeBorder, sizeof(glm::vec3));
        indices = (int *) malloc(indicesSize * sizeof(int));
        chunks = (Chunk *) malloc(numChunks * numChunks * sizeof(Chunk));

        seaweedPos = (glm::vec3 *) malloc(maxSeaweed * sizeof(glm::vec3));

        vertRowLen = verticesSize;

        // Generate data for each chunk
        for(int i = 0; i < numChunks; i++)
        {
          for(int j = 0; j < numChunks; j++)
          {
            chunkID = j + (i*numChunks);
            generateChunkPoints(j, i);
            setupChunk(j, i);
          }
        }
        std::cout << "Spawned: " << seaweedCount << std::endl;
        // Don't render the seaweed that didn't get a position
        this->maxSeaweed = seaweedCount;
    }

    // Setup Function For If The Map Already Exists
    Terrain(int seed, int numChunks, float heightScale, int indicesSize, vector<int> indicesIn, vector<vector<v::Vertex>> positionsIn)
    {
        this->seed = seed;
        this->numChunks = numChunks;
        this->chunkSize = chunkSize;
        this->heightScale = heightScale;
        this->indicesSize = indicesSize;


        // Mallocing
        verticesSize = (chunkSize+1) * (chunkSize+1);
        vertices = (Vertex **) malloc(numChunks * numChunks * sizeof(Vertex *));
        indices = (int *) malloc(indicesSize * sizeof(int));
        for(int i = 0; i < numChunks * numChunks; i++)
        {
          vertices[i] = (Vertex *) malloc(verticesSize * sizeof(Vertex));
        }
        chunks = (Chunk *) malloc(numChunks * numChunks * sizeof(Chunk));

        // Assigning Values From File
        for(int i = 0; i < numChunks * numChunks; i ++)
        {
          for(int j = 0; j < verticesSize; j++)
          {
            vertices[i][j] = positionsIn[i][j];

          }
        }

        for(int i = 0; i < indicesSize; i++)
        {
          indices[i] = indicesIn[i];
        }

        this->vertices = vertices;
        this->indices = indices;

        setupShader();


        for(int i = 0; i < numChunks; i++)
        {
          for(int j = 0; j < numChunks; j++)
          {
            chunkID = j + (i*numChunks);
            setupChunk(j, i);
          }
        }
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

          // Update this Vertexs' positon in correct memory space
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
            vertices[chunkID][count] = thisVertex;
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

      // Harder to be a mountain as you get further away
      height = - (.03 * toCenter) ;

      // // // Mountains
      for(int i = 0; i < octaves; i++)
      {
        height += ((glm::perlin(glm::vec2((float)posX/ns , (float)posZ/ns))+.707)/1.414) * hs;


        if(i == 0)
        {
          // Initial height less pronounces as further away
          height  += (.03 * toCenter);
          height *= 1./(exp(3. * toCenter/(((chunkSize * numChunks)/2.) * landPrevelence)));
          height -= (.03 * toCenter);
          if(height < waterLevel)
          {
            break;
          }
          // Set rockyness based on height
          hs = ((height/heightScale))*roughness;
        }

        // Adjust height and noiseScale
        hs *= .5;
        ns *= .5;

      }

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

      seaweedPosition(glm::vec3((posX-seed) * scale, height, (posZ-seed) * scale));

      return height;
    }

    float randFloat()
    {
      return ((float)rand()/RAND_MAX);
    }

    ////////////////////////////////////////
    // SEAWEED SPAWNING FUNCTIONS
    ////////////////////////////////////////
    void seaweedSetup()
    {
      float totalVertices = chunkSize * chunkSize * numChunks * numChunks;
      seaweedMin = 1.0 - (((float)maxSeaweed / (float)totalVertices) / (maxSeaweedSpawnChance / 2));

      while( seaweedMin < 0)
      {
        maxSeaweedSpawnChance += 0.01;
        seaweedMin = 1.0 - (((float)maxSeaweed / (float)totalVertices) / (maxSeaweedSpawnChance / 2));
      }
    }

    int spawnSeaweed(float noise, float min, float maxSpawnChance)
    {
      if(noise < min)
      {
        return 0.;
      }
      else
      {
        if((((noise - min) / (1.0 - min)) * maxSpawnChance) > randFloat())
        {
          didSpawn++;
          return 1.;
        }
        else
        {
          didntSpawn++;
          return 0.;
        }
      }
    }

    void seaweedPosition(glm::vec3 position)
    {
      float noise = (glm::perlin(glm::vec2((float)(position.x+seed)/(noiseScale/8.), (float)(position.z+seed)/(noiseScale/8.)))+.707)/1.414;

      if(seaweedCount < maxSeaweed)
      {
        if(spawnSeaweed(noise, seaweedMin, maxSeaweedSpawnChance) == 1.0)
        {
          if(position.y < -5.)
          {
            seaweedPos[seaweedCount] = position;
            seaweedCount++;
          }
        }
      }
    }

    ////////////////////////////////////////
    // END SEAWEED SPAWNING FUNCTIONS
    ////////////////////////////////////////

    // initializes all the buffer objects/arrays
    // inside the chunk from the current data
    void setupChunk(int x, int z)
    {
        // Create the chunk and put it in our chunks
        int chunkID = x + (z*numChunks);
        Chunk thisChunk = {.VAO = 0,
                           .VBO = 0,
                           .EBO = 0,
                           .points = vertices[chunkID],
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
        glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(Vertex), &vertices[chunkID][0], GL_STATIC_DRAW);
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
