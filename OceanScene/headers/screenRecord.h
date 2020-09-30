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

   float timeValue;
   std::stringstream frameStream;
   std::string frameString;
   int seed;
   int frame;
   int maxFrames;

   // Settings
   std::string framerate = "30";
   std::string imagesMergeCommand;

   ScreenRecord(int seed, int maxFrames)
   {
     std::cout << "Init" << '\n';
     this->seed = seed;
     this->maxFrames = maxFrames;
     time_t rawtime;
     struct tm * timeinfo;
     char buffer[80];
     time (&rawtime);
     timeinfo = localtime(&rawtime);
     strftime(buffer,sizeof(buffer),"_%d-%m-%Y_%H:%M",timeinfo);
     std::string currentTime(buffer);

     imagesMergeCommand = "cat ../videos/frame*.png | ffmpeg -y -f image2pipe -i - ../videos/section";
     joinVideoCommand = "ffmpeg -y -f concat -safe 0 -i ../videos/sections.txt -c copy -pix_fmt yuv420p -crf 18 ../videos/'" + std::to_string(seed) + "'" + currentTime + ".mp4";
   }

   void clearSections()
   {
      // Ran at the start to delete any spare frames
      std::cout << "Clearing file" << '\n';
      std::system("> ../videos/sections.txt");
   }


   void recordFrame(int frame)
   {
     timeValue = (1. / 60.) * frame;
     this->frame = frame;
     frameStream.str("");
     frameStream.clear();
     // Give all the frames a  unique name, ffmpeg breaks with frame numbers < 100 so 100 is added
    frameStream << std::setw(6) << std::setfill('0') << (int)frame + 100;
    frameString = frameStream.str();

    std::cout << std::to_string((int)(((float)frame/(float)maxFrames)*100.)) << "%" << '\n';
    saveScreenshot(("../videos/frame" + frameString + ".png").c_str());
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

  void imagesToVideoSegment()
  {
    // Converts the last 60 frames to a video and
    // deletes the frames
    std::cout << "recording" << std::endl;
    std::system((imagesMergeCommand + std::to_string(frame) + ".mp4").c_str());
    std::system("find ../videos/frame* -exec rm {} \\;");
    std::system(("echo \"file 'section" + std::to_string(this->frame) + ".mp4'\" >> ../videos/sections.txt").c_str());

  }

  void saveVideo()
  {

    std::cout << "Saving Video" << std::endl;
    std::system(joinVideoCommand.c_str());
    std::system("find ../videos/section* -exec rm {} \\;");
    std::cout << "Video Saved" << '\n';
  }

};


#endif // SCREENRECORD_H
