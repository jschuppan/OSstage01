#include "window.h"
#include "UI.h"
#include <iostream>
#include <curses.h>     // neede dfor Curses windowing
#include <stdarg.h>
#include <unistd.h>
#include <string>
using namespace std;

const int MAX_WINDOWS_THREADS = 6;

int main()
{
  initscr(); //strart curses
  refresh(); //refreshes virtual window
  UI userInf;


   char ch;
   for (int i=0;i<1000;i++) {
        if((ch = getch()) == 'a')
            userInf.addNewWindow();
        else if((ch = getch()) == 'q')
        {
            break;
        }
          sleep(1);
   }
  endwin();
}
