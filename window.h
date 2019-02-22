#pragma once

#include<iostream>
#include<curses.h>  //Used for curses windows
#include<stdarg.h> //formatting output to the window
#include<cstdlib>

class Window
{
private:
    WINDOW* window_ptr;
    int x,y;
    int width, height;

public:
	Window();
	Window(int height, int width, int starty, int startx);
	void create_window(int height, int width, int starty, int startx);
    void display_help();
    void write_window(const char* text);
    void write_window(int y, int x, const char* text);
    void moveWindow(int y, int x);
    void windowRefresh();
    void resizeWindow(int height, int width);

    //Accessors
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    WINDOW* getWindowPtr();

     //Mutators
    void setX(int x);
    void setY(int y);
    void setHeight(int height);
    void setWidth(int width);
    void setWindowPtr(WINDOW* window_ptr);



};
