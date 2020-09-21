#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#elif __LINUX__
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include<iostream>
#include<string.h>

// Finds the CWD on Windows or Linux
using namespace std;
char * get_current_dir() {
	char * cwd = GetCurrentDir( NULL, FILENAME_MAX );
	char* shortCwd = (char*) malloc((strlen(cwd) - 7) * sizeof(char));
	strcpy(shortCwd, cwd);
	std::cout << shortCwd << std::endl;
	return shortCwd;
   // in some real pointer hell here
}
