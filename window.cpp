#include "window.h"
#include <curses.h>     // needed for Curses windowing
#include <stdarg.h>
#include <iostream>

using namespace std;
//defualt constructer
Window:: Window()
{
    height = width = x = y = 0;
    window_ptr =NULL;
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
  write_window(1, 15, "c= clear screen");
  write_window(2, 15, "h= help screen");
  write_window(3, 15, "q= Quit");
  write_window(4, 15, "r=..Restart...");
  write_window(5, 15, "a= Add Thread");
  write_window(6, 15, "s=..Scheduler Dump1...");
  write_window(7, 15, "d=..Scheduler Dump2...");
  write_window(1, 35, "f=..Semaphore Dump1...");
  write_window(2, 35, "g=..Scheduler Dump2...");
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
    wclear(window_ptr);
    box(window_ptr,0,0);
    wrefresh(window_ptr);
}

void Window :: moveWindow(int y, int x)
{
    mvwin(window_ptr, y, x);
}

void Window :: createMaxSizeWindow()
{
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    create_window(maxY-1, maxX-1, 1,1);
}

void Window :: clearScreen()
{
  wclear(window_ptr);
  write_window(1,1,"Press h for help.");
  box(window_ptr,0,0);
  wrefresh(window_ptr);

}
bool Window :: operator==(Window& rhs)
{
  if(this->getWindowPtr() == rhs.getWindowPtr())
      return true;
  return false;
}

void deleteWindow()
{
  delete_win(window_ptr);
  window_ptr = NULL;
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
