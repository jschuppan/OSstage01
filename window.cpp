/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : window.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of window.h
============================================================================*/
#include "window.h"
#include <curses.h>      // needed for Curses windowing
#include <stdarg.h>

using namespace std;

//defualt constructer
Window:: Window()
{
    this->height = width = x = y = 0;
    this->window_ptr =NULL;
}

//constructor
Window:: Window(int height, int width, int starty, int startx,std::string name)
{
  setHeight(height);
  setWidth(width);
  setX(startx);
  setY(starty);
  create_window(height, width, y, x);
  this->name = name;
}

/*-----------------------------------------------------------------
Function      : create_window(int,int,int,int);
Parameters    : integers - height, width, y, x
Returns       : void
Details       : creates a window the size of the paramenters
------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------
Function      : write_window( const char*);
Parameters    : string, char[]
Returns       : void
Details       : writes the text to the window at default positions
------------------------------------------------------------------*/
void Window :: write_window(const char* text)
{
  wprintw(window_ptr, text);
  box(window_ptr, 0 , 0);
  wrefresh(window_ptr); // draw the window
}

/*-----------------------------------------------------------------
Function      : write_window(int x, int y, const char*);
Parameters    : integer- x,y : string, char[]
Returns       : void
Details       : writes the text to the window at paramenter location
------------------------------------------------------------------*/
void Window :: write_window(int y, int x, const char* text)
{
  mvwprintw(window_ptr, y, x, text);
  box(window_ptr, 0 , 0);
  wrefresh(window_ptr); // draw the window
}

/*-----------------------------------------------------------------
Function      : display_help();
Parameters    :
Returns       : void
Details       : displays a help menu to the window
------------------------------------------------------------------*/
void Window :: display_help()
{
  wclear(window_ptr);
  write_window(1, 1, "...Help...");
  write_window(2, 1, "0= Kill 0");
  write_window(3, 1, "1= Kill 1");
  write_window(4, 1, "2= Kill 2");
  write_window(1, 18, "3= Kill 3");
  write_window(2, 18, "4= Kill 4");
  write_window(3, 18, "5= Kill 5");
  write_window(4, 18, "c= clear screen");
  write_window(1, 35, "h= help screen");
  write_window(2, 35, "q= Quit");
  write_window(3, 35, "r=..Restart...");
  write_window(4, 35, "a= Add Thread");
  write_window(1, 50, "s=..Sched Dump1...");
  write_window(2, 50, "d=..Sched Dump2...");
  write_window(3, 50, "f=..Sema Dump1...");
  write_window(4, 50, "g=..Semap Dump2...");
}

/*-----------------------------------------------------------------
Function      : resizeWindow(int,height,int width);
Parameters    : integers- height, width
Returns       : void
Details       : resizes window to the given height and size
------------------------------------------------------------------*/
void Window :: resizeWindow(int height,int width)
{
   setHeight(height);
   wresize(window_ptr, height, width);
   windowRefresh();
}

/*-----------------------------------------------------------------
Function      : windowRefresh();
Parameters    :
Returns       : void
Details       : refreshes and draws a box around the window
------------------------------------------------------------------*/
void Window :: windowRefresh()
{
    //write_window(1,1,"HELLO");
    box(window_ptr,0,0);
    wrefresh(window_ptr);
}

/*-----------------------------------------------------------------
Function      : moveWindow(int y, int x);
Parameters    : integers - y,x
Returns       : void
Details       : moves a window to the given location
------------------------------------------------------------------*/
void Window :: moveWindow(int y, int x)
{
    mvwin(window_ptr, y, x);
}

/*-----------------------------------------------------------------
Function      : createMaxSizeWindow();
Parameters    :
Returns       : void
Details       : creates a window the same size as the stdscr
------------------------------------------------------------------*/
void Window :: createMaxSizeWindow()
{
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    create_window(maxY-1, maxX-1, 1,1);
}

/*-----------------------------------------------------------------
Function      : clearScreen;
Parameters    :
Returns       : void
Details       : clears the screen, draws a box around the window and
                refreshes it.
------------------------------------------------------------------*/
void Window :: clearScreen()
{
  wclear(window_ptr);
  write_window(1,1,"Press h for help.");
  box(window_ptr,0,0);
  wrefresh(window_ptr);

}

/*-----------------------------------------------------------------
Function      : operator==(Window &rhs);
Parameters    : Window object
Returns       : true if Windows point to the same curses WINDOW
Details       : checks if two Windows point to the same WINDOW
------------------------------------------------------------------*/
bool Window :: operator==(Window& rhs)
{
  if(this->getWindowPtr() == rhs.getWindowPtr())
      return true;
  return false;
}

/*-----------------------------------------------------------------
Function      : deleteWindow();
Parameters    :
Returns       : void
Details       : deletes the calling Window
------------------------------------------------------------------*/
void Window :: deleteWindow()
{
  delwin(window_ptr);
  window_ptr = NULL;
}

//Mutators
void Window:: setWindowPtr(WINDOW* window_ptr){this->window_ptr = window_ptr;}
void Window:: setX(int x){this->x = x;}
void Window:: setY(int y){this->y = y;}
void Window:: setHeight(int height){this->height = height;}
void Window:: setWidth(int width){this->width = width;}
void Window:: setName(std::string name){this->name = name;}

//Accessors
int Window:: getX(){return x;}
int Window:: getY(){return y;}
int Window:: getWidth(){return width;}
int Window:: getHeight(){return height;}
std::string Window:: getName(){return name;}
WINDOW* Window :: getWindowPtr(){return window_ptr;}
