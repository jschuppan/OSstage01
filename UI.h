/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : UI.h
Date          : Febuary 25, 2019
Purpose       : uses window objects to create a user interface
============================================================================*/
#ifndef UI_H
#define UI_H

//=================================
// Forward declarations
class MCB;

//=================================
// necessary includes
#include <iostream>
#include "window.h"
#include "linkedlist.h"

class UI
{
    private:
    /******************************* START PRIVATE MEMBERS ********************/
    linkedList <Window> windowList;
    //console max values
    int maxX,maxY,windowCreatedCount;
    /******************************* END PRIVATE MEMBERS ********************/

    public:
    /******************************* START PUBLIC MEMBERS *******************/
    UI();
    void update();
    void addStarterWindows();
    void addNewWindow();
    void resize();
    void clearConsoleScreen();
    Window* createMaxWindow();

    //mutators
    void setWindowCreatedCount(int windowCreatedCount);

    //accessors
    int getMaxX();
    int getMaxY();
    int getwindowCreatedCount();
    Window* getWindowCreated();
    Window* getWindowByID(int id);
    /******************************** END PUBLIC MEMBERS **********************/

};

#endif
