#ifndef ROCK_H
#define ROCK_H

#include <GLAD/glad.h>
#include <time.h>
#include <vector>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"

#include "verticies.h"
#include "shader.h"


struct RockVertex {
	glm::vec3 position;
	float noiseVal;
	glm::vec3 normal;
};

class Rock {
public:
	// number of cubes along an axis and overall length
	float nrCubes;
	float totalCubes;

	float nrVertices;
	float totalVertices;

	float length;

	RockVertex* grid;
	
	std::vector<RockVertex> rockVerticies;

	std::vector<float> triangleData;
	std::vector<float> indices;

	// position of the rock in world space
	glm::vec3 rockPos;

	// noise function data
	float seed;
	float isoLevel;

	// create a new shader object with the approriate vertex and fragment shaders
	Shader *rockShader = new Shader("rock.vs", "rock.fs");

	// openGL vertex data
	unsigned int VAO, VBO;

	Rock(float nVertices, float nLength, glm::vec3 nRockPos, float isoLevel) {
		this->nrVertices = nVertices;
		this->totalVertices = nrVertices * nrVertices * nrVertices;

		this->nrCubes = nrVertices - 1;
		this->totalCubes = (nrVertices - 1) * (nrVertices - 1) * (nrVertices - 1);

		this->length = nLength;


		this->rockPos = nRockPos;


		this->seed = 0.5;
		this->isoLevel = isoLevel;

		
		generateVertices();
		marchingCubes();

		for (int i = 0; i < triangleData.size(); i++) {
			std::cout << triangleData[i] << std::endl;
		}
		
		// generate vertex arrays and buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// bind vertex and buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(float), &triangleData.data()[0], GL_STATIC_DRAW);

		// set vertex attributes' layouts
		// vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time)
	{
		rockShader->use();

		rockShader->setMat4("projection", projection);
		rockShader->setMat4("view", view);
		rockShader->setMat4("model", model);

		rockShader->setVec4("cubeColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, triangleData.size());

		/*
		glm::mat4 vertexPos = glm::mat4(1.0f);
		for (unsigned int i = 0; i < totalVertices; i++)
		{
			vertexPos = glm::translate(model, verticies[i].position);
			rockShader->setMat4("model", vertexPos);

			if (verticies[i].noiseVal > isoLevel) {
			}

		}
		*/

		glBindVertexArray(0);
	}

	void SetupShader()
	{
		rockShader->use();
	}


private:

	//
	// get a grid of points around the object's position
	//
	void generateVertices()
	{

		// initialise vertexArray
		grid = (RockVertex*)malloc(totalVertices * sizeof(RockVertex));

		// get the starting position of the grid space from the rock's position
		glm::vec3 currPos = rockPos + glm::vec3(-length / 2.0f, -length / 2.0f, -length / 2.0f);

		// outer loop for x
		for (int i = 0; i < nrVertices; i++)
		{
			// loop for y
			for (int j = 0; j < nrVertices; j++)
			{
				// loop for z
				for (int k = 0; k < nrVertices; k++)
				{
					// set the vertex's position and get the associated seeded perlin noise scalar
					grid[index(i,j,k)].position = currPos;
					grid[index(i, j, k)].noiseVal = glm::perlin(currPos + glm::vec3(seed, seed, seed));
					currPos += glm::vec3(0.0f, 0.0f, length / nrVertices);
				}
				currPos += glm::vec3(0.0f, length / nrVertices, -length);
			}
			currPos += glm::vec3(length / nrVertices, -length, 0.0f);
		}
	}


	//
	// returns the index of the given x y and z values for nested for loop iterators
	//
	const int index(int x, int y, int z)
	{
		int index = z + y * nrVertices + x * (nrVertices * nrVertices);
		return  index;
	}

	//
	// marching cubes algorithm
	//
	void marchingCubes() {
		
		std::cout << "Marching cubes" << std::endl;
		int cubeIndex = 0;
		int nTriangles = 0;
		std::cout << "there are " << nrCubes << "cubes " << std::endl;

		// loop for x
		for (int i = 0; i < nrCubes; i++) 
		{
			std::cout << i << std::endl; 
			// loop for y
			for (int j = 0; j < nrCubes; j++)
			{
				std::cout << j << std::endl;
				// loop for z
				for (int k = 0; k < (int)nrCubes; k++) {
					std::cout << k << std::endl;
					// get cube vertices in order
					RockVertex marchingCube[8] = { 
						grid[index(i,j,k+1)  ], grid[index(i+1,j,k+1)	 ],
						grid[index(i+1,j,k)  ], grid[index(i,j,k)		 ],
						grid[index(i,j+1,k+1)], grid[index(i+1,j+1,k+1)],
						grid[index(i+1,j+1,k)], grid[index(i,j+1,k)	 ]
					};

					glm::vec3 cubeVertices[12] = {};

					// get the index for the edge table by getting an 8 bit number
					// from the noise values of the marching cube's vertices 
					cubeIndex = 0;
					if (marchingCube[0].noiseVal < isoLevel) cubeIndex |= 1;
					if (marchingCube[1].noiseVal < isoLevel) cubeIndex |= 2;
					if (marchingCube[2].noiseVal < isoLevel) cubeIndex |= 4;
					if (marchingCube[3].noiseVal < isoLevel) cubeIndex |= 8;
					if (marchingCube[4].noiseVal < isoLevel) cubeIndex |= 16;
					if (marchingCube[5].noiseVal < isoLevel) cubeIndex |= 32;
					if (marchingCube[6].noiseVal < isoLevel) cubeIndex |= 64;
					if (marchingCube[7].noiseVal < isoLevel) cubeIndex |= 128;

					// if the cube is completely in / out of the surface then skip
					// this marching cube
					if (edgeTable[cubeIndex] == 0)
						continue;
					// otherwise...
					if (edgeTable[cubeIndex] & 1)
						cubeVertices[0] =
							interpIntersect(marchingCube[0].position, marchingCube[1].position, marchingCube[0].noiseVal, marchingCube[1].noiseVal);
					if (edgeTable[cubeIndex] & 2)
						cubeVertices[1] =
							interpIntersect(marchingCube[1].position, marchingCube[2].position, marchingCube[1].noiseVal, marchingCube[2].noiseVal);
					if (edgeTable[cubeIndex] & 4)
						cubeVertices[2] =
							interpIntersect(marchingCube[2].position, marchingCube[3].position, marchingCube[2].noiseVal, marchingCube[3].noiseVal);
					if (edgeTable[cubeIndex] & 8)
						cubeVertices[3] =
							interpIntersect(marchingCube[3].position, marchingCube[0].position, marchingCube[3].noiseVal, marchingCube[0].noiseVal);
					if (edgeTable[cubeIndex] & 16)
						cubeVertices[4] =
							interpIntersect(marchingCube[4].position, marchingCube[5].position, marchingCube[4].noiseVal, marchingCube[5].noiseVal);
					if (edgeTable[cubeIndex] & 32)
						cubeVertices[5] =
							interpIntersect(marchingCube[5].position, marchingCube[6].position, marchingCube[5].noiseVal, marchingCube[6].noiseVal);
					if (edgeTable[cubeIndex] & 64)
						cubeVertices[6] =
							interpIntersect(marchingCube[6].position, marchingCube[7].position, marchingCube[6].noiseVal, marchingCube[7].noiseVal);
					if (edgeTable[cubeIndex] & 128)
						cubeVertices[7] =
							interpIntersect(marchingCube[7].position, marchingCube[4].position, marchingCube[7].noiseVal, marchingCube[4].noiseVal);
					if (edgeTable[cubeIndex] & 256)
						cubeVertices[8] =
							interpIntersect(marchingCube[0].position, marchingCube[4].position, marchingCube[0].noiseVal, marchingCube[4].noiseVal);
					if (edgeTable[cubeIndex] & 512)
						cubeVertices[9] =
							interpIntersect(marchingCube[1].position, marchingCube[5].position, marchingCube[1].noiseVal, marchingCube[5].noiseVal);
					if (edgeTable[cubeIndex] & 1024)
						cubeVertices[10] =
							interpIntersect(marchingCube[2].position, marchingCube[6].position, marchingCube[2].noiseVal, marchingCube[6].noiseVal);
					if (edgeTable[cubeIndex] & 2048)
						cubeVertices[11] =
							interpIntersect(marchingCube[3].position, marchingCube[7].position, marchingCube[3].noiseVal, marchingCube[7].noiseVal);

					// get the triangle data from the triangle table based on our cube vertices (get the number of triangles and their vertex positions
					// from the vertex data calculated just above
					for (int l = 0; triTable[cubeIndex][l] != -1; l += 3)
					{
						// push triangle data to a float vector
						pushCoords(cubeVertices[triTable[cubeIndex][l  ]]);
						pushCoords(cubeVertices[triTable[cubeIndex][l+1]]);
						pushCoords(cubeVertices[triTable[cubeIndex][l+2]]);
						
						nTriangles++;
					}
					std::cout << nTriangles << std::endl;
				}
			}
		}
	}

	//
	// Returns the interpolated position of a face's intersection with a cube edge depending
	// on the latter's vertices' scalar values and isoLevel
	//
	glm::vec3 interpIntersect(glm::vec3 vertex1, glm::vec3 vertex2, float scalar1, float scalar2)
	{
		glm::vec3 interpPos;
		float alpha = (isoLevel - scalar1) / (scalar2 - scalar1);
		interpPos.x = vertex1.x * alpha * (vertex2.x - vertex1.x);
		interpPos.y = vertex1.y * alpha * (vertex2.y - vertex1.y);
		interpPos.z = vertex1.z * alpha * (vertex2.z - vertex1.z);
		return interpPos;
	}

	//
	// Utility function to push coordinate components of triangle vertex positions
	//
	void pushCoords(glm::vec3 pos) {
		triangleData.push_back(pos.x);
		triangleData.push_back(pos.y);
		triangleData.push_back(pos.z);
	}


	glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		return glm::cross(c - a, b - a);
	}
};

#endif // ROCK_H
