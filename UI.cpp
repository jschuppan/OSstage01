#include "UI.h"
#include <iostream>


//default constructor
UI :: UI()
{
  getmaxyx(stdscr, maxY, maxX); // get console screen size
  wprintw(stdscr, "Screen Height = %d, Screen width = %d", maxY, maxX);
  refresh();
  windowCreatedCount = 0;
  addStarterWindows();
}

//adds UI windows
void UI :: addStarterWindows()
{
    //Header Window
      windowList.addToFront(Window(getMaxY() /5, getMaxX()-4, 2, 2), windowCreatedCount);
      windowCreatedCount++;

     //Control window
     int y2 = windowList.getDatumById(0)->getY() + windowList.getDatumById(0)->getHeight();
     windowList.addToEnd(Window(getMaxY() /5, getMaxX()*2/3-2, y2, 2),windowCreatedCount);
     windowCreatedCount++;

     int tempx =  windowList.getDatumById(1)->getX() +  windowList.getDatumById(1)->getWidth();
     int tempy =  windowList.getDatumById(1)->getY();

     //Input window
     windowList.addToEnd(Window(getMaxY() /5, getMaxX()/3-2, tempy, tempx), windowCreatedCount);
     windowCreatedCount++;

}

void UI :: addNewWindow()
{
     int height = getMaxY() /2;
     int width = (getMaxX()-3) /3;
     int y2;

     if(windowCreatedCount >= 9)
        return;
     if(windowCreatedCount == 3)
     {
         //Middle left Window
         y2 = windowList.getDatumById(0)->getY() + windowList.getDatumById(0)->getHeight();
         windowList.addToEnd(Window(height, width, y2, 2),windowCreatedCount);
         windowCreatedCount++;

         windowList.getDatumById(1)->moveWindow(windowList.getDatumById(3)->getY()+windowList.getDatumById(3)->getHeight(), 2);
         windowList.getDatumById(2)->moveWindow(windowList.getDatumById(3)->getY()+windowList.getDatumById(3)->getHeight(), windowList.getDatumById(1)->getX() + windowList.getDatumById(1)->getWidth());
         update();
     }
     else if(windowCreatedCount % 3 == 0)
     {
         resize();
         y2 = windowList.getDatumById(windowCreatedCount - 1)->getY() + windowList.getDatumById(windowCreatedCount - 1)->getHeight();
         windowList.addToEnd(Window(height/2, width, y2, 2), windowCreatedCount);
         windowCreatedCount++;
         update();
     }

     //SEGMENTATION FAULT HERE
     else if( windowCreatedCount <= 6)
     {
        //Middle windows
         int tempx = windowList.getDatumById(windowCreatedCount - 1)->getX() + windowList.getDatumById(windowCreatedCount - 1)->getWidth();
         int tempy = windowList.getDatumById(windowCreatedCount - 1)->getY();
         windowList.addToEnd(Window(height, width, tempy, tempx), windowCreatedCount);
         windowCreatedCount++;
     }

     else
     {
         //Middle windows
         int tempx = windowList.getDatumById(windowCreatedCount - 1)->getX() + windowList.getDatumById(windowCreatedCount - 1)->getWidth();
         int tempy = windowList.getDatumById(windowCreatedCount - 1)->getY();
         windowList.addToEnd(Window(height/2, width, tempy, tempx), windowCreatedCount);
         windowCreatedCount++;
     }
}


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
//Used to update size of console window
//refreshes all windows
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

void UI :: clearConsoleScreen()
{
    wclear(stdscr);
    wrefresh(stdscr);
}

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
