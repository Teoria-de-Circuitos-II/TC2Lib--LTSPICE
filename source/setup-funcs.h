#ifndef SETUP_FUNCS_H
#define SETUP_FUNCS_H

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
void print_menu(std::string);
void printWeather();
void color(std::string);
void clear_screen();

#endif // SETUP_FUNCS_H