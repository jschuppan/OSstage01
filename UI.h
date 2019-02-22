#ifndef UI_H
#define UI_H

#include<iostream>
#include "window.h"
#include "linkedlist.h"

class UI
{
    private:
    linkedList <Window> windowList;
    struct windowNode
    {
        Window* Win;
        windowNode* next;
    };
    windowNode* head;
    //console max values
    int maxX,maxY,windowsCreated;
    int windowCount;

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
