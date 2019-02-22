#ifndef UI_H
#define UI_H

#include<iostream>
#include "window.h"

class UI
{
    private:
    linkedList windowList;
    //console max values
    int maxX,maxY;

    public:
    UI();
    void update();
    void addStarterWindows();
    void addNewWindow();
    void resize();

    //mutators
    void setWindowCount(int windowCount);

    //accessors
    int getMaxX();
    int getMaxY();
    int getWindowCount();

};

#endif
