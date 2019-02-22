#include "UI.h"
#include<iostream>

//default constructor
UI :: UI()
{
  getmaxyx(stdscr, maxY, maxX); // get console screen size
  wprintw(stdscr, "Screen Height = %d, Screen width = %d", maxY, maxX);
  refresh();
  windowCreated = 0;
  addStarterWindows();
}

//adds UI windows
void UI :: addStarterWindows()
{
    //Header Window
      windowList.addToFront(Window(getMaxY() /5, getMaxX()-4, 2, 2), windowCreated);
      windowCreated++;

     //Control window
     int y2 = windowList.getDatumById(0)->getY() + windowList.getDatumById(0)->getHeight();
     windowList.addToEnd(Window(getMaxY() /5, getMaxX()*2/3-2, y2, 2),windowCreated);
     windowCreated++;

     int tempx =  windowList.getDatumById(1)->getX() +  windowList.getDatumById(1)->getWidth();
     int tempy =  windowList.getDatumById(1)->getY();

     //Input window
     windowList.addToEnd(Window(getMaxY() /5, getMaxX()/3-2, tempy, tempx), windowCreated);
     windowCreated++;

}

void UI :: addNewWindow()
{
     int height = getMaxY() /2;
     int width = (getMaxX()-3) /3;
     int y2;

     if(windowCreated >= 9)
        return;
     if(windowCreated == 3)
     {
         //Middle left Window
         y2 = windowList.getDatumById(0)->getY() + windowList.getDatumById(0)->getHeight();
         windowList.addToEnd(Window(height, width, y2, 2),windowCreated);
         windowCreated++;

         windowList.getDatumById(1)->moveWindow(windowList.getDatumById(3)->getY()+windowList.getDatumById(3)->getHeight(), 2);
         windowList.getDatumById(2)->moveWindow(windowList.getDatumById(3)->getY()+windowList.getDatumById(3)->getHeight(), windowList.getDatumById(1)->getX() + windowList.getDatumById(1)->getWidth());
         update();
         windowCreated++;
     }
     else if(windowCreated % 3 == 0)
     {
         resize();
         y2 = windowList.getDatumById(windowCreated - 1)->getY() + windowList.getDatumById(windowCreated - 1)->getHeight();
         windowList.addToEnd(Window(height/2, width, y2, 2), windowCreated);
         windowCreated++;
         update();
     }

     //SEGMENTATION FAULT HERE
     else if( windowCreated <= 6)
     {
        //Middle windows
         int tempx = windowList.getDatumById(windowCreated - 1)->getX() + windowList.getDatumById(windowCreated - 1)->getWidth();
         int tempy = windowList.getDatumById(windowCreated - 1)->getY();
         windowList.addToEnd(Window(height, width, tempy, tempx), windowCreated);
         windowCreated++;
     }
     /*
     else
     {
         //Middle windows
         int tempx = windowList.getDatumById(windowCreated - 1)->getX() + windowList.getDatumById(windowCreated - 1)->getWidth();
         int tempy = windowList.getDatumById(windowCreated - 1)->getY();
         windowList.addToEnd(Window(height/2, width, tempy, tempx), windowCreated);
         windowCreated++;
     }*/
}


void UI :: resize()
{
    if(windowCreated >= 9)
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

    for(int i = 0; i<windowCreated; i++)
    {
        windowList.getDatumById(i)->windowRefresh();
    }
}

//accessors
int UI :: getMaxY(){return maxY;}
int UI :: getMaxX(){return maxX;}
int UI :: getWindowCreated(){return windowCreated;}

//Mutators
void UI :: setWindowCreated(int windowCount){windowCreated = windowCreated;}
