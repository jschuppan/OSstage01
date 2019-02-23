#include "window.h"
#include <curses.h>     // neede dfor Curses windowing
#include <stdarg.h>
#include <iostream>

using namespace std;
//defualt constructer
Window:: Window()
{
  setX(2);
  setY(15);
  setHeight(10);
  setWidth(80);
}

//constructor
Window:: Window(int height, int width, int starty, int startx)
{
  setHeight(height);
  setWidth(width);
  setX(startx);
  setY(starty);
  create_window(height, width, y, x);
}

//creates a window and stores address into window_ptr
void Window:: create_window(int height, int width, int starty, int startx)
{
  WINDOW *Win;
  Win = newwin(height, width, starty, startx);
  scrollok(Win, TRUE); // Allow scrolling of the window
  scroll(Win); // scroll the window
  box(Win, 0 , 0); // 0, 0 gives default characters
  // for the vertical and horizontal lines
  wrefresh(Win); // draw the window
  setWindowPtr(Win);
}

//Writes to a window and refreshes the window
void Window :: write_window(const char* text)
{
  wprintw(window_ptr, text);
  box(window_ptr, 0 , 0);
  wrefresh(window_ptr); // draw the window
}

//Determine where in the window you want to write
void Window :: write_window(int y, int x, const char* text)
{
  mvwprintw(window_ptr, y, x, text);
  box(window_ptr, 0 , 0);
  wrefresh(window_ptr); // draw the window
}

//Will diaplay a help menu to the window
void Window :: display_help()
{
  wclear(window_ptr);
  write_window(1, 1, "...Help...");
  write_window(2, 1, "0= Kill 0");
  write_window(3, 1, "1= Kill 1");
  write_window(4, 1, "2= Kill 2");
  write_window(5, 1, "3= Kill 3");
  write_window(6, 1, "4= Kill 4");
  write_window(7, 1, "5= Kill 5");
  write_window(8, 1, "c= clear screen");
  write_window(9, 1, "h= help screen");
  write_window(10, 1, "q= Quit");
  write_window(11, 1, "d...Dump...");
  write_window(12, 1, "r...Restart...");
}

//meant to be used to change window size
void Window :: resizeWindow(int height,int width)
{
   setHeight(height);
   wresize(window_ptr, height, width);
   windowRefresh();
}

void Window :: windowRefresh()
{
    //write_window(1,1,"HELLO");
    box(window_ptr,0,0);
    wrefresh(window_ptr);
}

void Window :: moveWindow(int y, int x)
{
    mvwin(window_ptr, y, x);
}
//Mutators
void Window:: setWindowPtr(WINDOW* window_ptr){this->window_ptr = window_ptr;}
void Window:: setX(int x){this->x = x;}
void Window:: setY(int y){this->y = y;}
void Window:: setHeight(int height){this->height = height;}
void Window:: setWidth(int width){this->width = width;}

//Accessors
int Window:: getX(){return x;}
int Window:: getY(){return y;}
int Window:: getWidth(){return width;}
int Window:: getHeight(){return height;}
WINDOW* Window :: getWindowPtr(){return window_ptr;}
