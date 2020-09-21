#include "root_directory.h"

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include <iostream>
#include <string.h>

using namespace std;
char * get_current_dir() {
   char buff[FILENAME_MAX]; //create string buffer to hold path
   char * cwd = GetCurrentDir( buff, FILENAME_MAX );
   char shortCwd[strlen(cwd) - 7];
   strncpy(shortCwd, cwd, strlen(cwd) - 7);
   char * shortCwdReturn = shortCwd;
   return shortCwdReturn;
   // in some real pointer hell here
}
