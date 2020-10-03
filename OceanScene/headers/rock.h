#ifndef ROCK_H
#define ROCK_H

#include <glad/glad.h>
#include <time.h>
#include <vector>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"

#include "verticies.h"
#include "shader.h"

#define GEN_ROCK 0
#define GEN_PEBBLE 1

//
// Abstract away the class:
// Make the class capable of creating multiple copies of the same rock
// Make the class capable of creating shapes with a predefined base geometry
//


class Rock
{
public:
	// openGL vertex data
	unsigned int VAO, VBO;
	int triangleDataSize;

	//
	// rock arguments in order:
	// nVertices  -> number of vertices on one axis of the grid area
	// nLength    -> length/height/width of the grid area
	// nRockPos   -> Position of the rock in world space
	// isoLevel   -> value that determines whether a vertex is in or out of a surface for marching cube algorithm
	// noiseScale -> scalar used to modify perlin noise
	// genType    -> the kind of shape to generate either rock or pebble
	//
	Rock(float nVertices, float nLength, glm::vec3 nRockPos, float isoLevel, float noiseScale, int genType)
	{
		this->nrVertices = nVertices;
		this->totalVertices = nrVertices * nrVertices * nrVertices;

		this->nrCubes = nrVertices - 1;
		this->totalCubes = (nrVertices - 1) * (nrVertices - 1) * (nrVertices - 1);

		this->length = nLength;


		this->rockPos = nRockPos;


		this->seed = nRockPos.x + nRockPos.y + nRockPos.z;
		this->noiseScale = noiseScale;
		this->isoLevel = isoLevel;


		this->genType = genType;

		generateVertices();
		marchingCubes();
		setUpGL();
		triangleDataSize = triangleData.size();
	}

private:

	// number of cubes along an axis and overall length
	float nrCubes;
	float totalCubes;

	float nrVertices;
	float totalVertices;

	float length;

	ScalarVertex* grid;

	std::vector<float> triangleData;

	// position of the rock in world space
	glm::vec3 rockPos;

	// noise function data
	float seed;
	float noiseScale;
	float isoLevel;

	// type of grid vertex scalar attribution
	int genType;

	// create a new shader object with the approriate vertex and fragment shaders

	//Shader* normalShader = new Shader("normal.vs", "normal.fs", "normal.gs");


	//
	// Sets OpenGL data (VBO and VAO)
	//
	void setUpGL()
	{
		// generate vertex arrays and buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// bind vertex and buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(float), &triangleData.data()[0], GL_STATIC_DRAW);

		// set vertex attributes' layouts
		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	//
	// get a grid of points around the object's position
	//
	void generateVertices()
	{

		// initialise vertexArray
		grid = (ScalarVertex*)malloc(totalVertices * sizeof(ScalarVertex));

		// get the starting position of the grid space from the rock's position
		glm::vec3 currPos = rockPos;
		glm::vec3 rockCenter = rockPos + glm::vec3(length / 2.0f, length / 2.0f, length / 2.0f);
		glm::vec3 rockBottom = rockPos + glm::vec3(length / 2.0f, 0.f, length / 2.0f);
		float radius = length / 4.0f;

		// outer loop for y
		for (int i = 0; i < nrVertices; i++)
		{
			// loop for z
			for (int j = 0; j < nrVertices; j++)
			{
				// loop for x
				for (int k = 0; k < nrVertices; k++)
				{
					// set the vertex's position and get the associated seeded perlin noise scalar
					if (genType)
					{
						// GEN PEBBLE
						grid[index(i,j,k)].Position = currPos + rockPos;
						grid[index(i,j,k)].Scalar   = glm::perlin( noiseDistScale(currPos, rockCenter, 2.9f) * noiseScale* currPos + glm::vec3(seed, seed, seed));
					}
					else
					{
						// GEN ROCK
						grid[index(i, j, k)].Position = currPos + rockPos;

						//std::cout << currPos.x << ", " << currPos.y << ", " << currPos.z  << '\n';
						if ( (glm::distance(rockBottom, currPos) <= radius))
						{
							//std::cout << "    Yes " << '\n';
							grid[index(i, j, k)].Scalar = .4 + glm::perlin(noiseDistScale(currPos, rockCenter, 2.9f) * noiseScale * currPos + glm::vec3(seed, seed, seed));
							//std::cout << ": " << grid[index(i, j, k)].Scalar << '\n';
								//glm::perlin(noiseDistScale(currPos, rockCenter, 2.9f) * noiseScale * currPos + glm::vec3(seed, seed, seed));
						}
						else
						{
							grid[index(i, j, k)].Scalar = 0.0f;
						}
					}

					currPos += glm::vec3(length / nrVertices, 0.0f, 0.0f);
				}
				currPos += glm::vec3(-length, 0.0f, length / nrVertices);
			}
			currPos += glm::vec3(0.0f, length / nrVertices, -length);
		}
	}


	//
	// returns the index of the given x y and z values for nested for loop iterators
	//
	const int index(int a, int b, int c)
	{
		return a * (nrVertices * nrVertices) + b * nrVertices + c;
	}

	//
	// marching cubes algorithm
	//
	void marchingCubes()
	{
		int cubeIndex = 0;
		int nTriangles = 0;

		// loop for x
		for (int i = 0; i < nrCubes; i++)
		{
			// loop for y
			for (int j = 0; j < nrCubes; j++)
			{
				// loop for z
				for (int k = 0; k < nrCubes; k++) {

					// get a marching cube's vertices in order
					ScalarVertex marchingCube[8] = {
						grid[index(i, j, k)			   ], grid[index(i, j, k + 1)	 ],
						grid[index(i, j + 1, k + 1)	   ], grid[index(i, j + 1, k)	 ],
						grid[index(i + 1, j, k)		   ], grid[index(i + 1, j, k + 1)],
						grid[index(i + 1, j + 1, k + 1)], grid[index(i + 1, j + 1, k)]
					};

					glm::vec3 cubeVertices[12] = {};

					// Compare the scalar value associated with each vertex in the marching cube with the isoLevel
					// and if below a certain threshold set bit value of corresponding vertex to 1.
					// (8 bit number for 8 vertices in the cube and therefore 2^8 possible cube configurations)
					cubeIndex = 0;
					if (marchingCube[0].Scalar < isoLevel) cubeIndex |= 1;
					if (marchingCube[1].Scalar < isoLevel) cubeIndex |= 2;
					if (marchingCube[2].Scalar < isoLevel) cubeIndex |= 4;
					if (marchingCube[3].Scalar < isoLevel) cubeIndex |= 8;
					if (marchingCube[4].Scalar < isoLevel) cubeIndex |= 16;
					if (marchingCube[5].Scalar < isoLevel) cubeIndex |= 32;
					if (marchingCube[6].Scalar < isoLevel) cubeIndex |= 64;
					if (marchingCube[7].Scalar < isoLevel) cubeIndex |= 128;

					// if the cube is completely in / out of the surface then skip this marching cube.
					if (edgeTable[cubeIndex] == 0)
						continue;
					// otherwise...
					// Use the 8 bit cubeindex to determine which cube edges are intersected in the current
					// configuration and set the intersection points
					if (edgeTable[cubeIndex] & 1)
						cubeVertices[0] =
						interpIntersect(marchingCube[0].Position, marchingCube[1].Position, marchingCube[0].Scalar, marchingCube[1].Scalar);
					if (edgeTable[cubeIndex] & 2)
						cubeVertices[1] =
						interpIntersect(marchingCube[1].Position, marchingCube[2].Position, marchingCube[1].Scalar, marchingCube[2].Scalar);
					if (edgeTable[cubeIndex] & 4)
						cubeVertices[2] =
						interpIntersect(marchingCube[2].Position, marchingCube[3].Position, marchingCube[2].Scalar, marchingCube[3].Scalar);
					if (edgeTable[cubeIndex] & 8)
						cubeVertices[3] =
						interpIntersect(marchingCube[3].Position, marchingCube[0].Position, marchingCube[3].Scalar, marchingCube[0].Scalar);
					if (edgeTable[cubeIndex] & 16)
						cubeVertices[4] =
						interpIntersect(marchingCube[4].Position, marchingCube[5].Position, marchingCube[4].Scalar, marchingCube[5].Scalar);
					if (edgeTable[cubeIndex] & 32)
						cubeVertices[5] =
						interpIntersect(marchingCube[5].Position, marchingCube[6].Position, marchingCube[5].Scalar, marchingCube[6].Scalar);
					if (edgeTable[cubeIndex] & 64)
						cubeVertices[6] =
						interpIntersect(marchingCube[6].Position, marchingCube[7].Position, marchingCube[6].Scalar, marchingCube[7].Scalar);
					if (edgeTable[cubeIndex] & 128)
						cubeVertices[7] =
						interpIntersect(marchingCube[7].Position, marchingCube[4].Position, marchingCube[7].Scalar, marchingCube[4].Scalar);
					if (edgeTable[cubeIndex] & 256)
						cubeVertices[8] =
						interpIntersect(marchingCube[0].Position, marchingCube[4].Position, marchingCube[0].Scalar, marchingCube[4].Scalar);
					if (edgeTable[cubeIndex] & 512)
						cubeVertices[9] =
						interpIntersect(marchingCube[1].Position, marchingCube[5].Position, marchingCube[1].Scalar, marchingCube[5].Scalar);
					if (edgeTable[cubeIndex] & 1024)
						cubeVertices[10] =
						interpIntersect(marchingCube[2].Position, marchingCube[6].Position, marchingCube[2].Scalar, marchingCube[6].Scalar);
					if (edgeTable[cubeIndex] & 2048)
						cubeVertices[11] =
						interpIntersect(marchingCube[3].Position, marchingCube[7].Position, marchingCube[3].Scalar, marchingCube[7].Scalar);

					// Using the same cubeIndex, get the polygon data from triTable to determine how to order the intersection
					// vertices for each edge in the current configuration
					for (int l = 0; triTable[cubeIndex][l] != -1; l += 3)
					{
						// push triangle data to a float vector
						glm::vec3 faceNormal = normal(cubeVertices[triTable[cubeIndex][l + 2]], cubeVertices[triTable[cubeIndex][l + 1]], cubeVertices[triTable[cubeIndex][l + 0]]);

						pushVecComp(cubeVertices[triTable[cubeIndex][l + 2]]);
						pushVecComp(faceNormal);

						pushVecComp(cubeVertices[triTable[cubeIndex][l + 1]]);
						pushVecComp(faceNormal);

						pushVecComp(cubeVertices[triTable[cubeIndex][l + 0]]);
						pushVecComp(faceNormal);

						nTriangles++;
					}

				}
			}
		}
	}

	//
	// Returns the interpolated position of a face's intersection with a cube edge depending
	// on the latter's vertices' scalar values and isoLevel
	//
	const glm::vec3 interpIntersect(glm::vec3 vertex1, glm::vec3 vertex2, float scalar1, float scalar2)
	{
		if (abs(isoLevel - scalar1) < 0.00001)
			return vertex1;
		if (abs(isoLevel - scalar2) < 0.00001)
			return vertex2;
		if (abs(scalar1 - scalar2) < 0.00001)
			return vertex1;

		float alpha = (isoLevel - scalar1) / (scalar2 - scalar1);
		return glm::vec3(vertex1.x + alpha * (vertex2.x - vertex1.x), vertex1.y + alpha * (vertex2.y - vertex1.y), vertex1.z + alpha * (vertex2.z - vertex1.z));
	}

	//
	// Utility function for calculating the midpoint between two vertices
	//
	const glm::vec3 midPoint(glm::vec3 vertex1, glm::vec3 vertex2)
	{
		return glm::vec3(vertex2.x - vertex1.x, vertex2.y - vertex1.y, vertex2.z - vertex1.z);
	}

	//
	// Utility function to push coordinate components of triangle vertex positions
	//
	void pushVecComp(glm::vec3 pos)
	{
		triangleData.push_back(pos.x);
		triangleData.push_back(pos.y);
		triangleData.push_back(pos.z);
	}

	//
	// Utility function for calculating a face's normal
	//
	const glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		return glm::cross(b - a, b - c);
	}

	//
	// Utility function for printing a vector to terminal
	//
	void printVec(glm::vec3 vec)
	{
		std::cout << "Vector -> x: " << vec.x << " y: " << vec.y << " z: " << vec.z << std::endl;
	}

	//
	// Takes a vector as argument and returns a scalar value for multiplying noise value depending on
	// how close the given vertex is to the center of the grid as a sigmoid function
	//
	const float noiseDistScale(glm::vec3 vertex, glm::vec3 gridCenter, float alpha)
	{
		// get the corresponding value ranging between 0 and 1, accomodating for vertices that are out
		// of the range by setting a minimum value of 0
		float val = glm::max(1.0f - (glm::distance(vertex, gridCenter) / (length / 2.0f)), 0.0f);
		return 1.0f / (1.0f + glm::exp(-alpha * val + (alpha / 2.0f)));
	}
};

#endif // ROCK_H
