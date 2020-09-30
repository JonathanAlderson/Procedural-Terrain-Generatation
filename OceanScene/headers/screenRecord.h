#ifndef SCREENRECORD_H
#define SCREENRECORD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "stb_image_write.h"

class ScreenRecord
{
public:

   std::string audioCommand = "ffmpeg -y -i ../videos/'" + song + "'" + currentTime + ".mp4"  " -i ../data/'" + song + "'/'" + song + "'.mp3 -c copy -crf 18 ../videos/'" + song + "'" + currentTime + "_withAudio.mp4";
   std::string joinVideoCommand = "ffmpeg -y -f concat -safe 0 -i ../videos/sections.txt -c copy -pix_fmt yuv420p -crf 18 ../videos/'" + song + "'" + currentTime + ".mp4";

   ScreenRecord(int seed)
   {
     time_t rawtime;
     struct tm * timeinfo;
     char buffer[80];
     time (&rawtime);
     timeinfo = localtime(&rawtime);
     strftime(buffer,sizeof(buffer),"_%d-%m-%Y_%H:%M",timeinfo);

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

  void saveVideo(std::string mergeVideo, std::string addAudio, float record)
  {

    std::cout << "doing audio" << std::endl;

    if(record == 1.0)
    {
      std::system(mergeVideo.c_str());
      std::system(addAudio.c_str());
      std::system("find ../videos/section* -exec rm {} \\;");
    }
  }

};


#endif // SCREENRECORD_H
