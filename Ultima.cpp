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
void wrapperDump(Scheduler s, UI userInf, int level)

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
  while(1)
  {
    // std::cout << "START LOOP" << "\n";

       ID = s.running(ID);
       // sleep(1);
       if((ch = getch()) == 'a')
       {
           userInf.addNewWindow();
           s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
       }
       else if(ch == 'q')
       {
           break;
       }
       else if(ch == 's')
       {
           wrapperDump(s,userInf,1);
       }
      else if(ch == 'd')
      {
           wrapperDump(s,userInf,2);
      }
      else if(ch == 'f')
      {
           wrapperDump(s,userInf,3);
      }
      else if(ch == 'g')
      {
          wrapperDump(s,userInf,4);
      }
      else if(ch == 'c')
      {
          userInf.getDatumById(2)->clearScreen();
      }
        //  sleep(1);
  }
 endwin();
}

void wrapperDump(Scheduler s, UI userInf, int level)
{
    userInf.clearConsoleScreen();
    s.dump(userInf.createMaxSizeWindow(), level);
    sleep(5);
    userInf.clearConsoleScreen();
    userInf.update();

}
 void checkInput()
 {

  //       case 'c':           // clear the console window
  //           refresh(); // Clear the entire screen (in case it is corrupted)
  //           wclear(userInf.getWindowByID(2)); // Clear the Console window
  //           userInf.getWindowByID(2)->write_window( 1, 1, "Ultima # ");
  //           break;
  //       case 'h':
  //           userInf.getWindowByID(2)->display_help();
  //           userInf.getWindowByID(2)->write_window( 8, 1, "Ultima # ");
  //           break;
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
