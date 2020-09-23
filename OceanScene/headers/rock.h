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


class Rock {
public:
	// number of cubes along an axis and overall length
	float nrCubes;
	float totalCubes;

	float nrVertices;
	float totalVertices;

	float length;

	ScalarVertex* grid;
	
	std::vector<ScalarVertex> rockVerticies;

	std::vector<float> triangleData;
	std::vector<float> indices;

	// position of the rock in world space
	glm::vec3 rockPos;

	// noise function data
	float seed;
	float noiseScale;
	float isoLevel;

	// create a new shader object with the approriate vertex and fragment shaders
	Shader *rockShader = new Shader("rock.vs", "rock.fs");

	// openGL vertex data
	unsigned int VAO, VBO, cubeVAO, cubeVBO;

	Rock(float nVertices, float nLength, glm::vec3 nRockPos, float isoLevel, float noiseScale) {
		this->nrVertices = nVertices;
		this->totalVertices = nrVertices * nrVertices * nrVertices;

		this->nrCubes = nrVertices - 1;
		this->totalCubes = (nrVertices - 1) * (nrVertices - 1) * (nrVertices - 1);

		this->length = nLength;


		this->rockPos = nRockPos;


		this->seed = 0.3;
		this->noiseScale = noiseScale;
		this->isoLevel = isoLevel;

		
		generateVertices();
		marchingCubes();
		
		// generate vertex arrays and buffers
		glGenVertexArrays(1, &VAO);
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &cubeVBO);

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
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)3);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		/*
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW);

		// set vertex attributes' layouts
		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		*/
		
		SetupShader();
	}

	void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 camPos)
	{
		rockShader->use();

		rockShader->setMat4("projection", projection);
		rockShader->setMat4("view", view);
		rockShader->setMat4("model", model);
		rockShader->setVec3("viewPos", camPos);

		rockShader->setVec3("fragCol", glm::vec3(124.,124.,124.));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, triangleData.size());

		/*
		glBindVertexArray(cubeVAO);
		for (int i = 0 ; i < totalVertices; i++)
		{
			pos = glm::translate(model, grid[i].Position);
			rockShader->setMat4("model", pos);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glm::mat4 pos;
		// x in blue
		rockShader->setVec3("fragCol", glm::vec3(0., 0., 255.));
		pos = glm::translate(model, glm::vec3(3., 2., 2.));
		rockShader->setMat4("model", pos);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// y in green
		rockShader->setVec3("fragCol", glm::vec3(0., 255., 0.));
		pos = glm::translate(model, glm::vec3(2., 3, 2.));
		rockShader->setMat4("model", pos);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// z in red
		rockShader->setVec3("fragCol", glm::vec3(255., 0., 0.));
		pos = glm::translate(model, glm::vec3(2., 2., 3.));
		rockShader->setMat4("model", pos);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		*/

		glBindVertexArray(0);
	}

	void SetupShader()
	{
		rockShader->use();

		rockShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		rockShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		rockShader->setVec3("dirLight.diffuse", 1.f, 1.f, 1.f);
		rockShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	}


private:

	//
	// get a grid of points around the object's position
	//
	void generateVertices()
	{

		// initialise vertexArray
		grid = (ScalarVertex*)malloc(totalVertices * sizeof(ScalarVertex));

		// get the starting position of the grid space from the rock's position
		glm::vec3 currPos = rockPos;

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
					grid[index(i,j,k)].Position = currPos;
					grid[index(i,j,k)].Scalar   = glm::perlin( noiseScale * currPos + glm::vec3(seed, seed, seed));

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
	void marchingCubes() {
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


					// get cube vertices in order
					ScalarVertex marchingCube[8] = {
						grid[index(i,j,k)], grid[index(i,j,k + 1)],
						grid[index(i,j + 1,k + 1)], grid[index(i,j + 1,k)],
						grid[index(i + 1,j,k)], grid[index(i + 1,j,k + 1)],
						grid[index(i + 1,j + 1,k + 1)], grid[index(i + 1,j + 1,k)]
					};

					/*
					std::cout << "\n\nnew cube" << std::endl;
					bool test = false;
					for (int l = 0; l < 8; l++) {
						if (marchingCube[l].Scalar < isoLevel)
							test = true;
						else
							test = false;
						std::cout << "Vertex " << l << " " << marchingCube[l].Position.x << " " << marchingCube[l].Position.y << " " << marchingCube[l].Position.z << test << std::endl;
					}
					*/

					glm::vec3 cubeVertices[12] = {};

					// get the index for the edge table by getting an 8 bit number
					// from the noise values of the marching cube's vertices 
					cubeIndex = 0;
					if (marchingCube[0].Scalar < isoLevel) cubeIndex |= 1;
					if (marchingCube[1].Scalar < isoLevel) cubeIndex |= 2;
					if (marchingCube[2].Scalar < isoLevel) cubeIndex |= 4;
					if (marchingCube[3].Scalar < isoLevel) cubeIndex |= 8;
					if (marchingCube[4].Scalar < isoLevel) cubeIndex |= 16;
					if (marchingCube[5].Scalar < isoLevel) cubeIndex |= 32;
					if (marchingCube[6].Scalar < isoLevel) cubeIndex |= 64;
					if (marchingCube[7].Scalar < isoLevel) cubeIndex |= 128;

					// if the cube is completely in / out of the surface then skip
					// this marching cube
					if (edgeTable[cubeIndex] == 0)
						continue;
					// otherwise...
					if (edgeTable[cubeIndex] & 1)
						cubeVertices[0] =
						//midPoint(marchingCube[0].Position, marchingCube[7].Position);
						interpIntersect(marchingCube[0].Position, marchingCube[1].Position, marchingCube[0].Scalar, marchingCube[1].Scalar);
					if (edgeTable[cubeIndex] & 2)
						cubeVertices[1] =
						//midPoint(marchingCube[1].Position, marchingCube[2].Position);
						interpIntersect(marchingCube[1].Position, marchingCube[2].Position, marchingCube[1].Scalar, marchingCube[2].Scalar);
					if (edgeTable[cubeIndex] & 4)
						cubeVertices[2] =
						//midPoint(marchingCube[2].Position, marchingCube[3].Position);
						interpIntersect(marchingCube[2].Position, marchingCube[3].Position, marchingCube[2].Scalar, marchingCube[3].Scalar);
					if (edgeTable[cubeIndex] & 8)
						cubeVertices[3] =
						//midPoint(marchingCube[3].Position, marchingCube[0].Position);
						interpIntersect(marchingCube[3].Position, marchingCube[0].Position, marchingCube[3].Scalar, marchingCube[0].Scalar);
					if (edgeTable[cubeIndex] & 16)
						cubeVertices[4] =
						//midPoint(marchingCube[4].Position, marchingCube[5].Position);
						interpIntersect(marchingCube[4].Position, marchingCube[5].Position, marchingCube[4].Scalar, marchingCube[5].Scalar);
					if (edgeTable[cubeIndex] & 32)
						cubeVertices[5] =
						//midPoint(marchingCube[5].Position, marchingCube[6].Position);
						interpIntersect(marchingCube[5].Position, marchingCube[6].Position, marchingCube[5].Scalar, marchingCube[6].Scalar);
					if (edgeTable[cubeIndex] & 64)
						cubeVertices[6] =
						//midPoint(marchingCube[6].Position, marchingCube[7].Position);
						interpIntersect(marchingCube[6].Position, marchingCube[7].Position, marchingCube[6].Scalar, marchingCube[7].Scalar);
					if (edgeTable[cubeIndex] & 128)
						cubeVertices[7] =
						//midPoint(marchingCube[7].Position, marchingCube[4].Position);
						interpIntersect(marchingCube[7].Position, marchingCube[4].Position, marchingCube[7].Scalar, marchingCube[4].Scalar);
					if (edgeTable[cubeIndex] & 256)
						cubeVertices[8] =
						//midPoint(marchingCube[0].Position, marchingCube[4].Position);
						interpIntersect(marchingCube[0].Position, marchingCube[4].Position, marchingCube[0].Scalar, marchingCube[4].Scalar);
					if (edgeTable[cubeIndex] & 512)
						cubeVertices[9] =
						//midPoint(marchingCube[1].Position, marchingCube[5].Position);
						interpIntersect(marchingCube[1].Position, marchingCube[5].Position, marchingCube[1].Scalar, marchingCube[5].Scalar);
					if (edgeTable[cubeIndex] & 1024)
						cubeVertices[10] =
						//midPoint(marchingCube[2].Position, marchingCube[6].Position);
						interpIntersect(marchingCube[2].Position, marchingCube[6].Position, marchingCube[2].Scalar, marchingCube[6].Scalar);
					if (edgeTable[cubeIndex] & 2048)
						cubeVertices[11] =
						//midPoint(marchingCube[3].Position, marchingCube[7].Position);
						interpIntersect(marchingCube[3].Position, marchingCube[7].Position, marchingCube[3].Scalar, marchingCube[7].Scalar);

				// get the triangle data from the triangle table based on our cube vertices (get the number of triangles and their vertex positions
				// from the vertex data calculated just above
					for (int l = 0; triTable[cubeIndex][l] != -1; l += 3)
					{
						// push triangle data to a float vector
						glm::vec3 faceNormal = normal(cubeVertices[triTable[cubeIndex][l]], cubeVertices[triTable[cubeIndex][l + 1]], cubeVertices[triTable[cubeIndex][l + 2]]);

						pushCoords(cubeVertices[triTable[cubeIndex][l]]);
						//printVec(cubeVertices[triTable[cubeIndex][l]]);
						pushCoords(faceNormal);

						pushCoords(cubeVertices[triTable[cubeIndex][l + 1]]);
						//printVec(cubeVertices[triTable[cubeIndex][l + 1]]);
						pushCoords(faceNormal);

						pushCoords(cubeVertices[triTable[cubeIndex][l + 2]]);
						//printVec(cubeVertices[triTable[cubeIndex][l + 2]]);
						pushCoords(faceNormal);

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
	glm::vec3 interpIntersect(glm::vec3 vertex1, glm::vec3 vertex2, float scalar1, float scalar2)
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
	// Returns the midpoint of two vertices
	//
	glm::vec3 midPoint(glm::vec3 vertex1, glm::vec3 vertex2) {
		return glm::vec3(vertex2.x - vertex1.x, vertex2.y - vertex1.y, vertex2.z - vertex1.z);
	}

	//
	// Utility function to push coordinate components of triangle vertex positions
	//
	void pushCoords(glm::vec3 pos) {
		triangleData.push_back(pos.x);
		triangleData.push_back(pos.y);
		triangleData.push_back(pos.z);
	}

	//
	// Utility function for calculating a face's normal
	//
	glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		glm::vec3 normie = glm::cross(c - a, b - a);
		/*
		if (normie.x < 0 && normie.y < 0 && normie.z < 0) {
			normie.x *= -1;
			normie.y *= -1;
			normie.z *= -1;
		}
		*/
		if (normie.x < 0) {
			normie.x *= -1;
		}
		if (normie.y < 0) {
			normie.y *= -1;
		}
		if (normie.z < 0) {
			normie.z *= -1;
		}
		/*
		*/
		return normie;
	}

	//
	// Utility function for printing a vector to terminal
	//
	void printVec(glm::vec3 vec) 
	{
		std::cout << "Vector -> x: " << vec.x << " y: " << vec.y << " z: " << vec.z << std::endl;
	}
};

#endif // ROCK_H
