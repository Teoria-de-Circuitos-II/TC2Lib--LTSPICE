#ifndef SETUP_FUNCS_H
#define SETUP_FUNCS_H

#ifdef __unix__ /* __unix__ is usually defined by compilers targeting Unix systems */

#define OS_Windows 0

#elif defined(_WIN32) || defined(WIN32) /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */

#define OS_Windows 1

#endif

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <cstdio>

// Function declarations
bool initLib();
void doTheThing();
void setDarkTheme();
void setLightTheme();
void loadCustomComponents();
void loadCustomBackground();
void setPenWidth();

int music();
void print_menu(int, std::string);
void printWeather();
void color(std::string);
void clear_screen();
int hasResources();

#endif // SETUP_FUNCS_H