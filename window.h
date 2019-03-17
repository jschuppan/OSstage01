/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : window.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of window.h
============================================================================*/
#ifndef WINDOW_H
#define WINDOW_H

#include<curses.h>  //Used for curses windows
#include<stdarg.h> //formatting output to the window
#include<cstdlib>
#include<string>


class Window
{
private:
  /******************************* START PRIVATE MEMBERS ********************/
    WINDOW* window_ptr;
    int x,y;
    int width, height;
    std::string name;
    /******************************* END PRIVATE MEMBERS ********************/

public:
  /******************************* START PUBLIC MEMBERS *******************/
	Window();
	Window(int height, int width, int starty, int startx, std::string name);
	void create_window(int height, int width, int starty, int startx);
  void display_help();
  void write_window(const char* text);
  void write_window(int y, int x, const char* text);
  void moveWindow(int y, int x);
  void windowRefresh();
  void resizeWindow(int height, int width);
  void createMaxSizeWindow();
  void clearScreen();
  void deleteWindow();

  //Accessors
  int getX();
  int getY();
  int getWidth();
  int getHeight();
  std::string getName();
  WINDOW* getWindowPtr();

   //Mutators
  void setX(int x);
  void setName(std::string name);
  void setY(int y);
  void setHeight(int height);
  void setWidth(int width);
  void setWindowPtr(WINDOW* window_ptr);
  bool operator== (Window& rhs);
  /******************************** END PUBLIC MEMBERS **********************/
};

#endif
