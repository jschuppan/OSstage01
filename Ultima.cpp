#include "window.h"
#include "UI.h"
#include "linkedlist.h"
#include "scheduler.h"
#include "sema.h"
#include <iostream>
#include <curses.h>     // neede dfor Curses windowing
#include <stdarg.h>
#include <unistd.h>
#include <string>
#include <fstream>
using namespace std;

const int MAX_WINDOWS_THREADS = 6;

ofstream debugFile;

int main()
{
  initscr(); //strart curses
  refresh(); //refreshes virtual window
  linkedList <int> myList;
  Scheduler s;
  debugFile.open("debug.txt");
  UI userInf;
 int threadNum;

   char ch;
   for (int i=0;i<100;i++) {
        if((ch = getch()) == 'a')
        {
            userInf.addNewWindow();
            //debugFile << userInf.getwindowCreatedCount() << endl;
            s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
        }
        else if((ch = getch()) == 'q')
        {
            break;
        }
          sleep(1);
   }
endwin();
debugFile.close();
}
