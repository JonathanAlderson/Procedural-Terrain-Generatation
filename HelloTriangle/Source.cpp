#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Store the vertex shader source code as a string, dynamically compiled at run time
const char* vertexShaderSource = "#version 460 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

// Fragment shader code as a string
const char* firstFragmentShaderSource = "#version 460 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";

const char* secondFragmentShaderSource = "#version 460 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);\n"
	"}\0";

// Call back function that adjusts the viewport whenever the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	// Tell OpenGL the size of the viewport (our window) 
	glViewport(0, 0, width, height);
}

// Called to process user input, in this case signal to the window that it should close
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main() {

	// Instatiate the GLFW window
	glfwInit();
	// glfwWindowHint lets us configure the window
	// First argument is the opetion to be configured and the integer is the value of the option
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
	if (window  == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Make context of the window the context of the current thread
	glfwMakeContextCurrent(window);

	// Make sure GLAD is loaded to pass the OpenGL function pointers which are specific to the OS
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// **
	// Shader objects
	// **

	// Create the vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// tell OpenGL where the vertex shader's source code is and compile
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);


	// check if compilation was succesful
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// create the fragment shader
	unsigned int firstFragmentShader;
	firstFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(firstFragmentShader, 1, &firstFragmentShaderSource, NULL);
	glCompileShader(firstFragmentShader);

	glGetShaderiv(firstFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(firstFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMIPLATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int secondFragmentShader;
	secondFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(secondFragmentShader, 1, &secondFragmentShaderSource, NULL);
	glCompileShader(secondFragmentShader);

	glGetShaderiv(secondFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(secondFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMIPLATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int firstShaderProgram;
	firstShaderProgram = glCreateProgram();

	// link the program with the shaders 
	glAttachShader(firstShaderProgram, vertexShader);
	glAttachShader(firstShaderProgram, firstFragmentShader);
	glLinkProgram(firstShaderProgram);

	glGetProgramiv(firstShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(firstShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int secondShaderProgram;
	secondShaderProgram = glCreateProgram();

	// link the program with the shaders 
	glAttachShader(secondShaderProgram, vertexShader);
	glAttachShader(secondShaderProgram, secondFragmentShader);
	glLinkProgram(secondShaderProgram);

	glGetProgramiv(secondShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(secondShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(firstFragmentShader);
	glDeleteShader(secondFragmentShader);


	// **
	// Buffer objects and related data
	// **

	// Vertex input as an array, goes to the vertex shader	
	float verticesTri1[] = {
		-1.0f, 1.0f, 0.0f, // top right
		0.0f, 1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f, // bottom left
	};

	float verticesTri2[] = {
		1.0f, 1.0f, 0.0f,
		1.f, -1.f, 0.f,
		0.f, -1.f, 0.f
	};

	// index array for indexed drawing using elemebt buffer objects
	unsigned int indices[] = {
		0, 1, 2
	};
	unsigned int indicesTri2[] = {
		0, 1, 2
	};


	unsigned int VBO, VAO;
	//unsigned int EBO;

	unsigned int VAOS[2];
	unsigned int VBOS[2];

	// generate vertex array
	glGenVertexArrays(2, VAOS);
	// generate buffer objects
	glGenBuffers(2, VBOS);
	//glGenBuffers(1, &EBO);
	
	// Bind vertex array objet
	glBindVertexArray(VAOS[0]);

	// Bind the buffer to a type, there is only one type so any reference to that type references the bound buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
	// copy user data into the bound buffer (here our vertex data)
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTri1), verticesTri1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOS[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]); glBindVertexArray(VAOS[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTri2), verticesTri2, GL_STATIC_DRAW);

	// Bind element array buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// tell OpenGL how to interpret our data 
	// specify which vertex attribute (location 0 so 0), size of vertex attribute (vec3 so 3 values), type of data (float so GL_FLOAT),
	// specify if we want data normalised (applies to int and byte to set to GL_FALSE), specify the stride - space between consecutive
	// verte attributes, last is offset and must be void pointe hence cast
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// wireframe mode for draing polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

	// while loop - the render loop - to keep the application running
	while(!glfwWindowShouldClose(window))
	{
		// input 
		processInput(window);

		// rendering

		// Clear buffers at the start of the rendering
		glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);

		// activate linked shader program
		glUseProgram(firstShaderProgram);

		glBindVertexArray(VAOS[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(secondShaderProgram);

		glBindVertexArray(VAOS[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// There are two buffers being drawn to by the application. The front one contains the final
		// rendered image and the back one contains the image that is being drawn to. If a single buffer
		// was used, we would see a flickering of the final image being drawn as an artifact of the 
		// rendering process. We remove this by drawing to the back buffer and swapping once the 
		// rendering in the back buffer is completed.
		glfwSwapBuffers(window);
		// check for keyboard and mouse events (press, release, etc)
		glfwPollEvents();
	}

	// clean up the resources allocated to the application
	glfwTerminate();	
	return 0;
}