#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"



#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "terrain.h"
#include "fileLoader.h"
#include "seaweed.h"
#include "verticies.h"

#include "sceneSetup.h"
#include "texturesSetup.h"
#include "shadersSetup.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char *path);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const int MAX_SEAWEED = 500;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;





int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    //  Scene Setup
    // --------------------
    FileLoader fileSys = FileLoader();
    fileSys.WriteFile();

    //terrainSetup();

    // Terrain Setup
    float terrainHeight = 15.;
    Terrain terrain = Terrain(10, terrainHeight, 150., .2, 0.1, .8);
    Seaweed seaweed = Seaweed(100);


    // load textures
    // --------------------
    texturesSetup();


    // create shaders
    // ------------------------------------
    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");
    //Shader seaweedShader("seaweed.vs", "seaweed.fs");
    //Shader terrainShader("terrain.vs", "terrain.fs");
    Shader normalsShader("normal.vs", "normal.fs", "normal.gs");
    //shadersSetup();

    // shader configuration
    // --------------------
    lightingShaderSetup(lightingShader, camera);


    while (!glfwWindowShouldClose(window))
       {
           // per-frame time logic
           // --------------------
           float currentFrame = glfwGetTime();
           deltaTime = currentFrame - lastFrame;
           lastFrame = currentFrame;

           // input
           // -----
           processInput(window);

           // render
           // ------
           glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


           ///////////////////////////////////////////
           // Render logic
           ///////////////////////////////////////////


           // be sure to activate shader when setting uniforms/drawing objects
           lightingShader.use();
           lightingShader.setVec3("viewPos", camera.Position);
           lightingShader.setFloat("material.shininess", 32.0f);

           /*
              Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
              the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
              by defining light types as classes and set their values in there, or by using a more efficient uniform approach
              by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
           */
           // spotLight
           lightingShader.setVec3("spotLight.position", camera.Position);
           lightingShader.setVec3("spotLight.direction", camera.Front);

           // view/projection transformations
           glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
           glm::mat4 view = camera.GetViewMatrix();
           lightingShader.setMat4("projection", projection);
           lightingShader.setMat4("view", view);

           // world transformation
           glm::mat4 model = glm::mat4(1.0f);
           lightingShader.setMat4("model", model);

           // bind diffuse map
           glActiveTexture(GL_TEXTURE0);
           glBindTexture(GL_TEXTURE_2D, diffuseMap);
           // bind specular map
           glActiveTexture(GL_TEXTURE1);
           glBindTexture(GL_TEXTURE_2D, specularMap);





            // Draw Seaweed
            seaweed.Draw(model, view, projection, glfwGetTime());



            // Draw Terrain
            terrain.Draw(model, view, projection, glfwGetTime(), camera.Position);

            // normalsShader.use();
            // normalsShader.setMat4("projection", projection);
            // normalsShader.setMat4("view", view);
            // normalsShader.setMat4("model", model);
            // t.Draw();
            // then draw with normals


            // model = glm::mat4(1.0f);
            //
            // model = glm::mat4(1.0f);
            //
            // glBindVertexArray(terrainVAO);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
            // glDrawElements(GL_TRIANGLES, 3*2*6, GL_UNSIGNED_INT, 0);
            // glBindVertexArray(0);



            ///////////////////////////////////////////
            // END OF RENDER LOGIC
            ///////////////////////////////////////////

           // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
           // -------------------------------------------------------------------------------
           glfwSwapBuffers(window);
           glfwPollEvents();
       }

       // optional: de-allocate all resources once they've outlived their purpose:
       // ------------------------------------------------------------------------
       // cleanup done somewhere else

       // glfw: terminate, clearing all previously allocated GLFW resources.
       // ------------------------------------------------------------------
       glfwTerminate();
       return 0;
   }



/////////////////////////////
// CALLBACKS
/////////////////////////////

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


// Tried to move this elsewhere but
// ran straight into C++ stuff
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessBoost(deltaTime);
    else
        camera.ProcessBoost(-deltaTime);
}
