#ifndef UI_H
#define UI_H

#include<iostream>
#include "window.h"
#include "linkedlist.h"

class UI
{
    private:
    linkedList <Window> windowList;
    //console max values
    int maxX,maxY,windowCreated;

    public:
    UI();
    void update();
    void addStarterWindows();
    void addNewWindow();
    void resize();

    //mutators
    void setWindowCreated(int windowCreated);

    //accessors
    int getMaxX();
    int getMaxY();
    int getWindowCreated();

};

#endif
