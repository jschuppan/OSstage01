#include "window.h"
#include "UI.h"
#include<iostream>
#include <curses.h>     // neede dfor Curses windowing
#include <stdarg.h>
#include<unistd.h>
#include<string>
using namespace std;

void* myFunct(void* args);
int main()
{

    void* ptr;
    ptr = myFunct;
    
/*
  initscr(); //strart curses
  refresh(); //refreshes virtual window	
  

   UI userInf;
   char ch;
   for (int i=0;i<1000;i++) {
          if ((ch = getch()) == ERR) {
             continue;
          }
          else {
            userInf.addNewWindow();
          }
          sleep(3);
   }
  endwin();
  */
}

void* myFunct(void* args)
{
}
