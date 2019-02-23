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

int main()
{
  initscr(); //strart curses
  refresh(); //refreshes virtual window
  linkedList <int> myList;
  Scheduler s;
  ofstream debugUltima;
  debugUltima.open("debugUltima.txt");
  UI userInf;
  int threadNum;
  void checkInput();
  char ch;
  int ID = 0;
  userInf.addNewWindow();
  s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  userInf.addNewWindow();
  s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  userInf.addNewWindow();
  s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  // userInf.addNewWindow();
  // s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  // userInf.addNewWindow();
  // s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
  // userInf.addNewWindow();
  // s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));

  for (int i=0;i<1000;i++)
  {
    // std::cout << "START LOOP" << "\n";

       ID = s.running(ID);
       sleep(1);
       s.dump(0);
      //  if((ch = getch()) == 'a')
      //  {
      //      userInf.addNewWindow();
      //      s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
      //  }
      //  else if((ch = getch()) == 'q')
      //  {
      //      break;
      //  }
      //  else if((ch = getch()) == 'd')
      //  {
      //    s.dump(0);
      // }
          // debugUltima << "END LOOP" << "\n";
          //sleep(1);

  }



   // while(1) {
   //      //add window
   //      // if((ch = getch()) == ERR){
   //      //
   //      // }
   //      debugUltima << "START WHILE" << "\n";
   //
   //      if((ch = getch()) == 'a')
   //      {
   //          userInf.addNewWindow();
   //          //debugFile << userInf.getwindowCreatedCount() << endl;
   //          s.create_task(userInf.getWindowCreated(),userInf.getWindowByID(0),userInf.getWindowByID(1));
   //      }
   //      //quit
   //      else if((ch = getch()) == 'q')
   //      {
   //          break;
   //      }
   //      //dump
   //      else if((ch = getch()) == 'd')
   //      {
   //          s.dump(0);
   //      }
   //      debugUltima << "id for this run: " << ID << "\n";
   //      ID = s.running(ID);
   //      debugUltima << "scheduler returned: " << ID << "\n";
   //      debugUltima << "END WHILE" << "\n";
   //      sleep(1);
   //
   // }
endwin();
debugUltima.close();
}

// void checkInput()
// {
  // int processCounter;
  // while(input != 'q')
  //  {
  //    processCounter = s.getProcessCount();
  //    input = wgetch(Console_Win);
  //    switch(input)
  //    {
  //      case '0':
  //      case '1':
  //      case '2':
  //      case '3':
  //      case '4':
  //      case '5':
  //           if (input == '0' && processCounter >= -1) thread_args_0.kill_signal = true;
  //           else if (input == '1' && processCounter >= 1) thread_args_1.kill_signal = true;
  //           else if(input == '2' && processCounter >= 2)  thread_args_2.kill_signal = true;
  //           else if (input == '3'&& processCounter >= 3) thread_args_3.kill_signal = true;
  //           else if(input == '4'&& processCounter >= 4)  thread_args_4.kill_signal = true;
  //           else if(input == '5'&& processCounter >= 5)  thread_args_5.kill_signal = true;
  //           sprintf(buff, " %c\n", input);
  //           userInf.getWindowByID(2)->write_window( buff);
  //           sprintf(buff, " Kill = %c\n", input);
  //           userInf.getWindowByID(2)->write_window(buff);
  //           userInf.getWindowByID(2)->write_window(buff);
  //           sleep(4);
  //           wclear(userInf.getWindowByID(2));
  //           userInf.getWindowByID(2)->write_window( 1, 1, "Ultima # ");
  //           break;
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
