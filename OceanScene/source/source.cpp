#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"



#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"
#include "waterFrameBuffers.h"
#include "sceneSetup.h"
#include "texturesSetup.h"
#include "shadersSetup.h"
#include "postProcessing.h"

#define SCREENRECORDING false
#define RECORDINGFRAMES 500
#define CAMERADOLLY false

// #if SCREENRECORDING == true
#include "screenRecord.h"
// #endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char *path);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 25.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float camReflectDist = 0.;
float underwater;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int frame = 0;
float frameTime;


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

    // Water Setup
    // --------------------
    WaterFrameBuffers * fbos = new WaterFrameBuffers(SCR_WIDTH, SCR_HEIGHT);

    //  Scene Setup
    // --------------------
    Scene scene = Scene(fbos, &camera, CAMERADOLLY);

    // Recording Setup
    // --------------------
    ScreenRecord * screenRecord;
    int maxFrames = 999999999;
    int framerate = 30;
    if(SCREENRECORDING)
    {
      maxFrames = RECORDINGFRAMES;
      std::cout << "Screen Recording Enabled" << '\n';
      screenRecord = new ScreenRecord(scene.seed, maxFrames, framerate);
      std::thread startUp(&ScreenRecord::clearSections, screenRecord);
      startUp.join();
    }

    // Post Processing Setup
    // --------------------------
    PostProcessing postProcessing = PostProcessing();

    while (!glfwWindowShouldClose(window) && frame <= maxFrames)
       {
           // per-frame time logic
           // --------------------
           frameTime = (SCREENRECORDING) ? ((1. / (float)framerate) * frame) : glfwGetTime();

           deltaTime = frameTime - lastFrame;
           lastFrame = frameTime;

           // input
           // -----
           processInput(window);

           // auto camera movement
           scene.MoveCamera(&camera, frameTime);

           // render
           // ------
           glEnable(GL_CLIP_DISTANCE0);

           // render reflection texture
           fbos->bindReflectionFrameBuffer();
           // move camera
           camReflectDist = 2 * camera.Position.y;
           underwater = (camera.Position.y < 0) ? -1 : (camera.Position.y > 0);

           camera.Position.y -= camReflectDist;
           camera.invertPitch();

           scene.DrawNoWater(SCR_WIDTH, SCR_HEIGHT, camera, frameTime, glm::vec4(0., 1. * underwater, 0., 0.));
           fbos->unbindCurrentFrameBuffer();
           camera.Position.y += camReflectDist;
           camera.invertPitch();

           // render refraction texture
           fbos->bindRefractionFrameBuffer();
           scene.DrawNoWater(SCR_WIDTH, SCR_HEIGHT, camera, frameTime, glm::vec4(0., -1. * underwater, 0., 0.));
           fbos->unbindCurrentFrameBuffer();


           // // render to post processing texture
           fbos->bindPostProcessingFrameBuffer();
           scene.Draw(SCR_WIDTH, SCR_HEIGHT, camera, frameTime, glm::vec4(0., -1., 0., 999999.));
           fbos->unbindCurrentFrameBuffer();

           postProcessing.Draw(fbos->getPostProcessingTexture(), fbos->getPostProcessingDepthTexture());


           // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
           // -------------------------------------------------------------------------------
           glfwSwapBuffers(window);
           glfwPollEvents();

           if(SCREENRECORDING)
           {
             screenRecord->recordFrame(frame);
             frame++;
             if((int)frame % 60 == 0 && frame > 0){ std::thread addImagesThread(&ScreenRecord::imagesToVideoSegment, screenRecord); addImagesThread.join(); }
           }

       }

       // glfw: terminate, clearing all previously allocated GLFW resources.
       // ------------------------------------------------------------------
       glfwTerminate();

       // Save the video
       // -----------------------------------
        if(SCREENRECORDING)
        {
          std::cout << "Saving Final Video" << '\n';
          std::thread compileVideoThread(&ScreenRecord::saveVideo, screenRecord);
          compileVideoThread.join();
          std::cout << "Saved" << '\n';
        }

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
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ShowPosition();
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        camera.ShowRotation();
}
