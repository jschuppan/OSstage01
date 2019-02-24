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
void wrapperDump(Scheduler &s, UI &userInf, int level);

int main()
{


                               // If no input is ready, getch returns ERR
  initscr(); //strart curses
  refresh(); //refreshes virtual window

  cbreak();                     // disable line buffering
  noecho();                    // disable automatic echo of characters read by getch(), wgetch()
  nodelay(stdscr, true);  // nodelay causes getch to be a non-blocking call.

  linkedList <int> myList;
  Scheduler s;
  UI userInf;
  char ch;
  int ID = 2;

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
    // std::cout << "START LOOP" << "\n";

       ID = s.running(ID);
       // sleep(1);
       ch= getch();
       switch (ch)
       {
         case 'a':
         {
             userInf.addNewWindow();
             s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
             break;
         }
         case 'q':
         {
             break;
         }
         case 's':
         {
             wrapperDump(s,userInf,1);
             break;
         }
        case 'd':
        {
             wrapperDump(s,userInf,2);
             break;
        }
        case 'f':
        {
             wrapperDump(s,userInf,3);
             break;
        }
        case 'g':
        {
            wrapperDump(s,userInf,4);
            break;
        }
        case  'c':
        {
            userInf.getWindowByID(2)->clearScreen();
            break;
        }
        case 'h':
        {
            userInf.getWindowByID(2)->display_help();
            userInf.getWindowByID(2)->write_window( 8, 1, "Ultima # ");
            break;
        }
       }

        //  sleep(1);
  }
 endwin();
}

void wrapperDump(Scheduler &s, UI &userInf, int level)
{
    userInf.clearConsoleScreen();
    s.dump(userInf.createMaxWindow(), level);
    sleep(5);
    userInf.clearConsoleScreen();
    userInf.update();

}
 // void checkInput()
 // {
  //       case 'q':           // end the loop, and end the program.
  //           userInf.getWindowByID(2)->write_window(" Quiting the main program....\n" );
  //           break;
  //       case 'd':
  //           s.stop();
  //
  //          s.dump(userInf.create_window());
  //       case 'r':
  //           s.restart();
  //       case ERR:
  //       // if wgetch() return ERR, that means no keys were pressed
  //       // earlier we enabled non-blocking input using nodelay() see above
  //       // this allows the program to continue to inspect the keyboard without
  //       // having to wait for the key to be pressed.
  //           break;
  //           default:
  //           sprintf(buff, " %c\n", input);
  //           userInf.getWindowByID(2)->write_window( buff);
  //           userInf.getWindowByID(2)->write_window( " -Invalid Command\n");
  //           userInf.getWindowByID(1)->write_window( buff);
  //           userInf.getWindowByID(1)->write_window( " -Invalid Command\n");
  //           userInf.getWindowByID(2)->write_window(" Ultima # ");
  //           break;
  //       }
//         sleep(1);
//     }
// }
