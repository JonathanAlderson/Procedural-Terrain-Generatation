#ifndef ROCK_H
#define ROCK_H

#include <GLAD/glad.h>
#include <time.h>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"

#include "verticies.h"
#include "shader.h"


struct RockVertex {
	glm::vec3 position;
	float noiseVal;
};


class Rock {
public:
	// number of cubes along an axis and overall length
	float nrVertices;
	float totalVertices;
	float length;
	RockVertex*vertexPositions;

	// position of the rock in world space
	glm::vec3 rockPos;

	// seed for noise function
	float seed;

	// create a new shader object with the approriate vertex and fragment shaders
	Shader *rockShader = new Shader("rock.vs", "rock.fs");


	// openGL vertex data
	unsigned int VAO, VBO;

	Rock(float nVertices, float nLength, glm::vec3 nRockPos) {
		this->nrVertices = nVertices;
		this->totalVertices = nrVertices * nrVertices * nrVertices;
		this->length = nLength;

		this->rockPos = nRockPos;

		this->seed = 0.00123214f;

		GetVertices();
	}

	void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time)
	{
		// generate vertex arrays and buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// bind vertex and buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW);

		// set vertex attributes' layouts
		// vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normals
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);
		//glEnableVertexAttribArray(1);

		rockShader->use();

		rockShader->setMat4("projection", projection);
		rockShader->setMat4("view", view);
		rockShader->setMat4("model", model);

		
		glm::mat4 vertexPos = glm::mat4(1.0f);
		for (unsigned int i = 0; i < totalVertices; i++)
		{
			rockShader->setVec4("cubeColor", glm::vec4(vertexPositions[i].noiseVal, vertexPositions[i].noiseVal, vertexPositions[i].noiseVal, 1.0f));
			vertexPos = glm::translate(model, vertexPositions[i].position);
			rockShader->setMat4("model", vertexPos);
			if (vertexPositions[i].noiseVal > 0.5) {
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		//int stop;
		//if (i % 9 == 0) {
			//std::cout << "new grid row" << std::endl;
		//}
		//std::cout << "index " << i << " " << vertexPositions[i].x << " " << vertexPositions[i].y << " " << vertexPositions[i].z << std::endl;
		//cin >> stop;

		glBindVertexArray(0);
	}

	void SetupShader()
	{
		rockShader->use();
	}


private:

	void GetVertices()
	{
		//
		// get a grid of points around the object's position
		//

		// initialise vertexArray
		vertexPositions = (RockVertex*)malloc(totalVertices * sizeof(RockVertex));

		// get the starting position of the grid space
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
					vertexPositions[getVertexIndex(i,j,k)].position = currPos;
					vertexPositions[getVertexIndex(i, j, k)].noiseVal = glm::perlin(currPos + glm::vec3(seed, seed, seed));
					std::cout << vertexPositions[getVertexIndex(i, j, k)].noiseVal << std::endl;
					currPos = currPos + glm::vec3(0.0f, 0.0f, length / nrVertices);
				}
				currPos = currPos + glm::vec3(0.0f, length / nrVertices, -length);
			}
			currPos = currPos + glm::vec3(length / nrVertices, -length, 0.0f);
		}
	}

	// returns the index of the given x y and z values for nested for loop iterators
	const int getVertexIndex(int x, int y, int z)
	{
		int index = z + y * nrVertices + x * (nrVertices * nrVertices);
		return  index;
	}
};













#endif // ROCK_H
