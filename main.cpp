#include "window.h"
#include "UI.h"
#include<iostream>
#include <curses.h>     // neede dfor Curses windowing
#include <stdarg.h>
#include<unistd.h>
#include<string>
#include "linkedlist.h"
using namespace std;

int main()
{

  initscr(); //strart curses
  refresh(); //refreshes virtual window

//  Window d1;
  //mylist.addToFront(d1, 1);
  //mylist.getDatumById(1)->write_window(1,1,"HELLO");



   UI userInf;
   
   char ch;
   for (int i=0;i<1000;i++) {
          if ((ch = getch()) == ERR) {
             continue;
          }
          else {
            //userInf.addNewWindow();
          }
          sleep(3);
   }

  endwin();
}
