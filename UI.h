#ifndef UI_H
#define UI_H

// 8==D~{(.)}

#include<iostream>
#include "window.h"
#include "linkedlist.h"

class UI
{
    private:
    linkedList <Window> windowList;
    //console max values
    int maxX,maxY,windowCreatedCount;

    public:
    UI();
    void update();
    void addStarterWindows();
    void addNewWindow();
    void resize();

    //mutators
    void setWindowCreatedCount(int windowCreatedCount);

    //accessors
    int getMaxX();
    int getMaxY();
    int getwindowCreatedCount();
    Window* getWindowCreated();


};

#endif
