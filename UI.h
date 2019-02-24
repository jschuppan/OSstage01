#pragma once

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
    void clearConsoleScreen();
    WINDOW* createMaxWindow();

    //mutators
    void setWindowCreatedCount(int windowCreatedCount);

    //accessors
    int getMaxX();
    int getMaxY();
    int getwindowCreatedCount();
    Window* getWindowCreated();
    Window* getWindowByID(int id);


};
