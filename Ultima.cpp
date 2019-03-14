/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : Ultima.cpp
Date          : Febuary 25, 2019
Purpose       : Driver for project ULTIMA
============================================================================*/
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
#include <ctime>
using namespace std;

const int MAX_WINDOWS_THREADS = 6;
void wrapperDump(Scheduler &s, UI &userInf, int level);

int main()
{
  initscr(); //strart curses
  refresh(); //refreshes virtual window

  // disable line buffering
  cbreak();
  // disable automatic echo of characters read by getch(), wgetch()
  noecho();
  // nodelay causes getch to be a non-blocking call.
  nodelay(stdscr, true);

  linkedList <int> myList;
  Scheduler s;
  UI userInf;
  char ch;
  std::clock_t garbageTimerStart = std::clock();
  double timeElapsed;
  void* ID = NULL;

  //Create starter threads and windows
  userInf.addNewWindow();
  s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  userInf.addNewWindow();
  s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  userInf.addNewWindow();
  s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));

  //loop until q is pressed
  while(ch != 'q')
  {
       // start our scheduler which returns the ID
       // to the next node and continous round-robin style
       ID = s.running(ID);

       // run garbage_collect() every 30 seconds
       timeElapsed = ((std::clock() - garbageTimerStart) / (double)CLOCKS_PER_SEC);
       if (timeElapsed > 30) {
         s.garbage_collect();
         garbageTimerStart = std::clock();
       }

       //Get user input
       ch = getch();
       switch (ch)
       {
         //Add new window
         case 'a':
         {
            usleep(1000);
             userInf.addNewWindow();
             s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
             break;
         }
         //Quit program
         case 'q':
         {
             break;
         }
         //Dump(level1)
         case 's':
         {
             wrapperDump(s,userInf,1);
             break;
         }
        //Dump(level2)
        case 'd':
        {
             wrapperDump(s,userInf,2);
             break;
        }
        //Dump(level3)
        case 'f':
        {
             wrapperDump(s,userInf,3);
             break;
        }
       //Dump(level4)
        case 'g':
        {
            wrapperDump(s,userInf,4);
            break;
        }
        //clear console screen
        case  'c':
        {
            usleep(1000);
            userInf.getWindowByID(2)->clearScreen();
            break;
        }
        //Display help
        case 'h':
        {
            usleep(1000);
            userInf.getWindowByID(2)->display_help();
            userInf.getWindowByID(2)->write_window( 8, 1, "Ultima # ");
            break;
        }
        //Kill thread
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        {

            if(s.getTCBList().getDatumById((int)ch-'0') == NULL)
            {
                usleep(1000);
                userInf.getWindowByID((int)ch-'0')->write_window(1,1,"ERROR DELETING THREAD");
            }

            else
                s.getTCBList().getDatumById((int)ch-'0')->setState(3);
          break;
        }
        case 'j':
        {
          s.forceWrite(rand()%5);
          break;
        }
        //Resume5 running
        case 'r':
        {
              s.resume();
        }
      }
    }
    //end curses
    endwin();
}

/*-----------------------------------------------------------------
Function      : wrapperDump(Scheduler &s, UI &userInf, int level);
Parameters    : Scheduler,UI, integer level
Returns       : void
Details       : A wrapper function to encapsulate the call to dump
------------------------------------------------------------------*/
void wrapperDump(Scheduler &s, UI &userInf, int level)
{
    //pause program
    s.stop();
    sleep(1);
    userInf.clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();
    s.dump(Win, level);
    //display dump window for 8 seconds
    sleep(3);
    userInf.update();

}
