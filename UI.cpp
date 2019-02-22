#include "UI.h"
#include<iostream>

//default constructor
UI :: UI()
{
  getmaxyx(stdscr, maxY, maxX); // get console screen size
  head = NULL;
  wprintw(stdscr, "Screen Height = %d, Screen width = %d", maxY, maxX);
  refresh();
  windowCount = 0;
  addStarterWindows();
}

//adds UI windows
void UI :: addStarterWindows() 
{
  //can only create windows if no windows exist
  if(head == NULL)
    { 
    
    //Header Window
      head = new windowNode;
      head->next = NULL;
      head-> Win = new Window(getMaxY() /5, getMaxX()-4, 2, 2);
      windowNode* temp = head;
      windowCount++;
      head->Win->write_window("Header");
        
     //Control window
     temp->next = new windowNode;
     int y2 = temp->Win->getY() + temp->Win->getHeight();
     temp->next->Win = new Window(getMaxY() /5, getMaxX()*2/3-2, y2, 2);
     temp = temp->next;
     temp->next = NULL;
     windowCount++;
     
     int tempx = temp->Win->getX() + temp->Win->getWidth();
     int tempy = temp->Win->getY();
     
     //Input window
     temp->next = new windowNode;
     tempx = temp->Win->getX() + temp->Win->getWidth();
     tempy = temp->Win->getY();
     temp->next->Win = new Window(getMaxY() /5, getMaxX()/3-2, tempy, tempx);
     temp = temp->next;
     temp->next = NULL;
     windowCount++;
     //temp->Win->write_window(windowCount);
     //wrefresh(temp->Win);
    }
}

void UI :: addNewWindow()
{
     int height = getMaxY() /2;
     int width = (getMaxX()-3) /3;
     int count = 0;
     int y2;
     windowNode* temp = head;
     while(temp->next)
     {
        temp = temp->next;
     }
     
     if(windowCount >= 9)
        return;
     if(windowCount == 3)
     {
         //Middle left Window
         temp->next = new windowNode;
         y2 = head->Win->getY() + head->Win->getHeight();
         temp->next->Win = new Window(height, width, y2, 2);
         temp = temp->next;
         temp->next = NULL;
         
         head->next->Win->moveWindow(temp->Win->getY()+temp->Win->getHeight(), 2);
         head->next->next->Win->moveWindow(temp->Win->getY()+temp->Win->getHeight(), head->next->Win->getX() + head->next->Win->getWidth());
         update();
         windowCount++;
     }
     else if(windowCount % 3 == 0)
     {
         resize();
         temp->next = new windowNode;
         y2 = temp->Win->getY() + temp->Win->getHeight();
         temp->next->Win = new Window(height/2, width, y2, 2);
         temp = temp->next;
         temp->next = NULL;
         update();
         windowCount++;
     }
     else if( windowCount <= 6)
     {
        //Middle windows
         temp->next = new windowNode;
         int tempx = temp->Win->getX() + temp->Win->getWidth();
         int tempy = temp->Win->getY();
         temp->next->Win = new Window(height, width, tempy, tempx);
         temp = temp->next;
         temp->next = NULL;
         windowCount++;
     }
     else
     {
         //Middle windows
         temp->next = new windowNode;
         int tempx = temp->Win->getX() + temp->Win->getWidth();
         int tempy = temp->Win->getY();
         temp->next->Win = new Window(height/2, width, tempy, tempx);
         temp = temp->next;
         temp->next = NULL;
         windowCount++;
     }
}


void UI :: resize()
{
    windowNode* temp = head;
    if(windowCount >= 9)
        return;
    else
    {
        temp = temp->next->next->next;
        temp->Win->resizeWindow(temp->Win->getHeight()/2, temp->Win->getWidth());
        temp->next->Win->resizeWindow(temp->next->Win->getHeight()/2, temp->Win->getWidth());
        temp->next->next->Win->resizeWindow(temp->next->next->Win->getHeight()/2, temp->Win->getWidth());
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
    
    windowNode* temp = head;
    while(temp)
    {    
        if(temp == NULL)
            continue;
        temp->Win->windowRefresh();
        temp = temp->next;
    }
}

//accessors
int UI :: getMaxY(){return maxY;}
int UI :: getMaxX(){return maxX;}
int UI :: getWindowCount(){return windowCount;}

//Mutators
void UI :: setWindowCount(int windowCount){this->windowCount = windowCount;}
