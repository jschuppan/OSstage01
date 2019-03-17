/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : UI.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of UI.h
============================================================================*/
#include "UI.h"
#include <iostream>
#include<cstdlib>
#include <string>

//default constructor
UI :: UI()
{
  // get console screen size
  getmaxyx(stdscr, maxY, maxX);
  wprintw(stdscr, "Screen Height = %d, Screen width = %d", maxY, maxX);
  refresh();
  windowCreatedCount = 0;
  addStarterWindows();
}

/*-----------------------------------------------------------------
Function      : addStarterWindows();
Parameters    :
Returns       : void
Details       : creates three starter windows
------------------------------------------------------------------*/
void UI :: addStarterWindows()
{
    //Header Window
      windowList.addToFront(Window(getMaxY() /5, getMaxX()-4, 2, 2,"HeaderWin "), windowCreatedCount);
      windowCreatedCount++;

     //Control window
     int y2 = windowList.getDatumById(0)->getY() + windowList.getDatumById(0)->getHeight();
     windowList.addToEnd(Window(getMaxY() /5, getMaxX()*1/3-2, y2, 2,"RunningWin "),windowCreatedCount);
     windowCreatedCount++;

     int tempx =  windowList.getDatumById(1)->getX() +  windowList.getDatumById(1)->getWidth();
     int tempy =  windowList.getDatumById(1)->getY();

     //Input window
     windowList.addToEnd(Window(getMaxY() /5, getMaxX()*2/3-2, tempy, tempx,"ConsoleWin "), windowCreatedCount);
     windowList.getDatumById(2)->write_window(1,1,"Press h for help.");
     windowCreatedCount++;

}

/*-----------------------------------------------------------------
Function      : addNewWindow();
Parameters    :
Returns       : void
Details       : creates three starter windows
------------------------------------------------------------------*/
void UI :: addNewWindow()
{
     int height = getMaxY() /2;
     int width = (getMaxX()-3) /3;
     int y2;
     std::string tempString = "ThreadWin " + windowCreatedCount;
     //Only create 9 windows
     if(windowCreatedCount >= 9)
        return;

     //used for window alignment
     if(windowCreatedCount == 3)
     {
         //Middle left Window
         y2 = windowList.getDatumById(0)->getY() + windowList.getDatumById(0)->getHeight();
         windowList.addToEnd(Window(height, width, y2, 2,tempString),windowCreatedCount);
         windowCreatedCount++;

         windowList.getDatumById(1)->moveWindow(windowList.getDatumById(3)->getY()+windowList.getDatumById(3)->getHeight(), 2);
         windowList.getDatumById(2)->moveWindow(windowList.getDatumById(3)->getY()+windowList.getDatumById(3)->getHeight(), windowList.getDatumById(1)->getX() + windowList.getDatumById(1)->getWidth());
         update();
     }
     else if(windowCreatedCount % 3 == 0)
     {
         resize();
         y2 = windowList.getDatumById(windowCreatedCount - 1)->getY() + windowList.getDatumById(windowCreatedCount - 1)->getHeight();
         windowList.addToEnd(Window(height/2, width, y2, 2,tempString), windowCreatedCount);
         windowCreatedCount++;
         update();
     }
     else if( windowCreatedCount <= 6)
     {
        //Middle windows
         int tempx = windowList.getDatumById(windowCreatedCount - 1)->getX() + windowList.getDatumById(windowCreatedCount - 1)->getWidth();
         int tempy = windowList.getDatumById(windowCreatedCount - 1)->getY();
         windowList.addToEnd(Window(height, width, tempy, tempx,tempString), windowCreatedCount);
         windowCreatedCount++;
     }

     else
     {
         //Middle windows
         int tempx = windowList.getDatumById(windowCreatedCount - 1)->getX() + windowList.getDatumById(windowCreatedCount - 1)->getWidth();
         int tempy = windowList.getDatumById(windowCreatedCount - 1)->getY();
         windowList.addToEnd(Window(height/2, width, tempy, tempx,tempString), windowCreatedCount);
         windowCreatedCount++;
     }
}

/*-----------------------------------------------------------------
Function      : resize();
Parameters    :
Returns       : void
Details       : shrinks the middle 3 windows by half
------------------------------------------------------------------*/
void UI :: resize()
{
    if(windowCreatedCount >= 9)
        return;
    else
    {
        windowList.getDatumById(3)->resizeWindow(windowList.getDatumById(3)->getHeight()/2, windowList.getDatumById(3)->getWidth());
        windowList.getDatumById(4)->resizeWindow(windowList.getDatumById(4)->getHeight()/2, windowList.getDatumById(4)->getWidth());
        windowList.getDatumById(5)->resizeWindow(windowList.getDatumById(5)->getHeight()/2, windowList.getDatumById(5)->getWidth());
    }
}


/*-----------------------------------------------------------------
Function      : update();
Parameters    :
Returns       : void
Details       : clears console screen and updates all windows
------------------------------------------------------------------*/
void UI :: update()
{
    //clear and refresh standard screen
    wclear(stdscr);
    wprintw(stdscr, "Screen Height = %d, Screen width = %d", maxY, maxX);
    wrefresh(stdscr);

    for(int i = 0; i<windowCreatedCount; i++)
    {
        windowList.getDatumById(i)->windowRefresh();
    }
}

/*-----------------------------------------------------------------
Function      : clearConsoleScreen();
Parameters    :
Returns       : void
Details       : clears console screen and refreshes
------------------------------------------------------------------*/
void UI :: clearConsoleScreen()
{
    wclear(stdscr);
    wrefresh(stdscr);
}

/*-----------------------------------------------------------------
Function      : createMaxWindow();
Parameters    :
Returns       : void
Details       : calls function create createMaxSizeWindow in Window class
------------------------------------------------------------------*/
Window* UI :: createMaxWindow()
{
  Window *win;
  win->createMaxSizeWindow();
  return win;
}

//accessors
int UI :: getMaxY(){return maxY;}
int UI :: getMaxX(){return maxX;}
int UI :: getwindowCreatedCount(){return windowCreatedCount;}
Window* UI :: getWindowCreated() {return windowList.getDatumById((windowCreatedCount - 1));}
Window* UI :: getWindowByID(int id){return windowList.getDatumById(id);}

//Mutators
void UI :: setWindowCreatedCount(int windowCreatedCount){this->windowCreatedCount = windowCreatedCount;}
