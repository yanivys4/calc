#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <conio.h>

#else
#include <sys/ioctl.h>
#include <termios.h>
#define NCURSES_NOMACROS 
#include <ncurses.h>
int _kbhit();
int getche();
#endif


int getCharInput();
void clearOutput();
void print(const char &);