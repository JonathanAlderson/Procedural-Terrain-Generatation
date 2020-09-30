#ifndef SCREENRECORD_H
#define SCREENRECORD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <iomanip>
#include <thread>

class ScreenRecord
{
public:

   std::string joinVideoCommand;
   std::string imagesMergeCommand = "cat ../videos/frame*.png | ffmpeg -r " + framerate + " -i - -c:v libx264 -vf \"pad=ceil(iw/2)*2:ceil(ih/2)*2\"  -crf 18 -framerate " + framerate + " -pix_fmt yuv420p ../videos/section" + frameString + ".mp4";

   float timeValue;
   std::stringstream frameStream;
   std::string frameString;

   // Settings
   std::string framerate = "30";


   ScreenRecord(int seed)
   {
     time_t rawtime;
     struct tm * timeinfo;
     char buffer[80];
     time (&rawtime);
     timeinfo = localtime(&rawtime);
     strftime(buffer,sizeof(buffer),"_%d-%m-%Y_%H:%M",timeinfo);
     std::string currentTime(buffer);

     joinVideoCommand = "ffmpeg -y -f concat -safe 0 -i ../videos/sections.txt -c copy -pix_fmt yuv420p -crf 18 ../videos/'" + std::to_string(seed) + "'" + currentTime + ".mp4";
     imagesMergeCommand = "cat ../videos/frame*.png | ffmpeg -r " + framerate + " -i - -c:v libx264 -vf \"pad=ceil(iw/2)*2:ceil(ih/2)*2\"  -crf 18 -framerate " + framerate + " -pix_fmt yuv420p ../videos/section";


   }

   void recordFrame(int frame)
   {
     timeValue = (1. / 60.) * frame;
     frameStream.str("");
     frameStream.clear();
     // Give all the frames a  unique name, ffmpeg breaks with frame numbers < 100 so 100 is added
    frameStream << std::setw(6) << std::setfill('0') << (int)frame + 100;

    saveScreenshot(("../videos/frame" + frameString + ".png").c_str());
    if((int)frame % 60 == 0 && frame > 0){ std::thread addImagesThread(imagesToVideoSegment, imagesMergeCommand, 1, frameString); addImagesThread.join(); }
   }

  int saveScreenshot(const char *filename)
  {
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);

      int x = viewport[0];
      int y = viewport[1];
      int width = viewport[2];
      int height = viewport[3];

      char *data = (char*) malloc((size_t) (width * height * 3)); // 3 components (R, G, B)

      if (!data)
          return 0;

      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

      int saved = stbi_write_png(filename, width, height, 3, data, 0);

      free(data);

      return saved;
  }

  void imagesToVideoSegment(std::string command, float record, std::string frame)
  {
    // Converts the last 60 frames to a video and
    // deletes the frames
    std::cout << "recording" << std::endl;
    if(record == 1.0)
    {

      std::system(command.c_str());
      std::system("find ../videos/frame* -exec rm {} \\;");
      std::system(("echo \"file 'section" + frame + ".mp4'\" >> ../videos/sections.txt").c_str());
    }
  }

  void saveVideo()
  {

    std::cout << "doing audio" << std::endl;

    if(record == 1.0)
    {
      std::system(joinVideoCommand.c_str());
      std::system("find ../videos/section* -exec rm {} \\;");
    }
  }

};


#endif // SCREENRECORD_H
