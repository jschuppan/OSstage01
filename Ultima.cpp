/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : Ultima.cpp
Date          : Febuary 25, 2019
Purpose       : Driver for project ULTIMA
============================================================================*/


//=================================
// Forward declarations
class Window;
class MCB;

//=================================
// necessary includes
#include <iostream>
#include <curses.h>     // needed for nCurses
#include <stdarg.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <ctime>
#include "UI.h"
#include "scheduler.h"
#include "sema.h"
#include "IPC.h"
#include "mem_mgr.h"
#include "string.h"
#include "UFS.h"

static const char alphabet[26] = {'a','b','c','d','e','f','g','h','i',
                    'j','k','l','m','n','o','p','q','r',
                    's','t','u','v','w','x','y','z'};
const int DUMP_SLEEP = 8;
const int MAX_WINDOWS_THREADS = 6;
const int HEADER_WIN = 0;
const int RUNNING_WINDOW = 1;
const int CONSOLE_WINDOW = 2;
int fileOpen1 =0;

void wrapperDump(Scheduler* s, UI* userInf, int level);
void setMCB(MCB* mcb);
void endlessLoop(MCB* mcb);
void mem_wrapperDump(Scheduler* s, Mem_Mgr* m, UI* userInf);
void IPCwrapperDump(Scheduler* s, UI* userInf, int level);
void filewrapperDump(UFS* ufs, UI* userInf);
void filewrapperDumpDir(UFS* ufs, UI* userInf);
void filewrapperDumpDir(UFS* ufs, UI* userInf,int threadID);




//main
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

  MCB* mcb = new MCB;

  setMCB(mcb);
  //Create starter threads and windows
  mcb->userInf->addNewWindow();
  mcb->s->create_task(mcb->userInf->getWindowCreated(),mcb->userInf->getWindowByID(HEADER_WIN),mcb->userInf->getWindowByID(RUNNING_WINDOW));
  mcb->userInf->addNewWindow();
  mcb->s->create_task(mcb->userInf->getWindowCreated(),mcb->userInf->getWindowByID(HEADER_WIN),mcb->userInf->getWindowByID(RUNNING_WINDOW));
  mcb->userInf->addNewWindow();
  mcb->s->create_task(mcb->userInf->getWindowCreated(),mcb->userInf->getWindowByID(HEADER_WIN),mcb->userInf->getWindowByID(RUNNING_WINDOW));

  endlessLoop(mcb);

  //end curses
  endwin();
}

/*-----------------------------------------------------------------
Function      : wrapperDump(Scheduler &s, UI &userInf, int level);
Parameters    : Scheduler,UI, integer level
Returns       : void
Details       : A wrapper function to encapsulate the call to dump
------------------------------------------------------------------*/

void wrapperDump(Scheduler* s, UI* userInf, int level)
{
    //pause program
    s->stop();
    sleep(1);
    userInf->clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();
    s->dump(Win, level);
    //display dump window for DUMP_SLEEP seconds
    sleep(DUMP_SLEEP);
    Win->deleteWindow();
    userInf->update();

}


/*-----------------------------------------------------------------
Function      : mem_wrapperDump(Scheduler &s, UI &userInf, int level);
Parameters    : Scheduler,UI, integer level
Returns       : void
Details       : A wrapper function to encapsulate the call to dump
------------------------------------------------------------------*/
void mem_wrapperDump(Scheduler* s, Mem_Mgr* m, UI* userInf)
{
    //pause program
    s->stop();
    sleep(1);
    userInf->clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();
    m->mem_dump(Win);
    //display dump window for DUMP_SLEEP seconds
    sleep(DUMP_SLEEP);
    Win->deleteWindow();
    userInf->update();

}

/*-----------------------------------------------------------------
Function      : IPCwrapperDump(Scheduler &s, UI &userInf, int level);
Parameters    : Scheduler,UI, integer level
Returns       : void
Details       : A wrapper function to encapsulate the call to dump
------------------------------------------------------------------*/
void IPCwrapperDump(Scheduler* s, UI* userInf, int level)
{
    sleep(1);
    //suserInf->clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();

    s->messageDump(Win, level);
    //display dump window for DUMP_SLEEP seconds
    sleep(DUMP_SLEEP);
    Win->deleteWindow();
    userInf->update();
    //Win->clearScreen();

}

void filewrapperDump(UFS* ufs, UI* userInf)
{
    sleep(1);
    //suserInf->clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();

    ufs->dump(Win);
    //display dump window for DUMP_SLEEP seconds
    sleep(DUMP_SLEEP);
    Win->deleteWindow();
    userInf->update();
    //Win->clearScreen();

}

void filewrapperDumpDir(UFS* ufs, UI* userInf)
{
    sleep(1);
    //suserInf->clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();

    //ufs->dir(Win);
    //display dump window for DUMP_SLEEP seconds
    sleep(DUMP_SLEEP);
    Win->deleteWindow();
    userInf->update();
    //Win->clearScreen();

}

void filewrapperDumpDir(UFS* ufs, UI* userInf, int threadID)
{
    sleep(1);
    //suserInf->clearConsoleScreen();
    Window * Win = new Window();
    //create new window to dump to
    Win->createMaxSizeWindow();

    //ufs->dir(Win,threadID);
    //display dump window for DUMP_SLEEP seconds
    sleep(DUMP_SLEEP);
    Win->deleteWindow();
    userInf->update();
    //Win->clearScreen();

}
/*-----------------------------------------------------------------
Function      : endlessLoop()
Parameters    :
Returns       : void
Details       : A wrapper function to encapsulate an endless loop
------------------------------------------------------------------*/
void endlessLoop(MCB* mcb)
{
  void* ID = NULL;
  char ch;
  std::clock_t garbageTimerStart = std::clock();
  double timeElapsed;
//loop until q is pressed
  while(ch != 'q')
  {

      //usleep(20000);
       // start our scheduler which returns the ID
       // to the next node and continous round-robin style
       ID = mcb->s->running(ID);

       // run garbage_collect() every 30 seconds
       timeElapsed = ((std::clock() - garbageTimerStart) / (double)CLOCKS_PER_SEC);
       if (timeElapsed > 30) {
         mcb->s->garbage_collect();
         garbageTimerStart = std::clock();
       }

       //Get user input
       ch = getch();
       //mcb->userInf->getWindowByID(CONSOLE_WINDOW)->write_window(ch + " ");
       switch (ch)
       {
         //Add new window
         case 'a':
         {
            usleep(1000);
             mcb->userInf->addNewWindow();
             mcb->s->create_task(mcb->userInf->getWindowCreated(), mcb->userInf->getWindowByID(HEADER_WIN), mcb->userInf->getWindowByID(RUNNING_WINDOW));
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
             wrapperDump(mcb->s, mcb->userInf,1);
             break;
         }
        //Dump(level2)
        case 'S':
        {
             wrapperDump(mcb->s, mcb->userInf,2);
             break;
        }
        //Dump(level3)
        case 'd':
        {
             wrapperDump(mcb->s, mcb->userInf,3);
             break;
        }
       //Dump(level4)
        case 'D':
        {
            wrapperDump(mcb->s,mcb->userInf,4);
            break;
        }
        //message Dump(level1)
         case 'm':
         {
             IPCwrapperDump(mcb->s,mcb->userInf,1);
             break;
         }
         //message Dump(level2)
          case 'M':
          {
              mem_wrapperDump(mcb->s,mcb->mem_mgr,mcb->userInf);
              break;
          }
        //clear console screen
        case  'c':
        {
            usleep(1000);
            mcb->userInf->getWindowByID(CONSOLE_WINDOW)->clearScreen();
            break;
        }
        //Display help
        case 'h':
        {
            usleep(1000);
            mcb->userInf->getWindowByID(CONSOLE_WINDOW)->display_help();
            mcb->userInf->getWindowByID(CONSOLE_WINDOW)->write_window( 8, 1, "Ultima # ");
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

            if(mcb->s->getTCBList().getDatumById((int)ch-'0') == NULL)
            {
                usleep(1000);
                mcb->userInf->getWindowByID((int)ch-'0')->write_window(1,1,"ERROR DELETING THREAD");
            }

            else
                mcb->s->getTCBList().getDatumById((int)ch-'0')->setState(3);
          break;
        }
        case 'f':
        {
          mcb->s->forceWrite(rand()%5);
          break;
        }
        case 'k':
        {
          char* c = "This space is mine";
          mcb->mem_mgr->mem_write(mcb->s->getThreadInfo().getDatumById(1)->getMemHandle(), 90 , strlen(c), c, 1);
          break;
        }
        case 'K':
        {
          char* c = "This space is mine";
          mcb->mem_mgr->mem_write(mcb->s->getThreadInfo().getDatumById(1)->getMemHandle(), 1, strlen(c), c, 1);
          mcb->mem_mgr->mem_write(mcb->s->getThreadInfo().getDatumById(0)->getMemHandle(), 1, strlen(c), c, 0);

          break;
        }
        case 'z':
        {
          mcb->s->getThreadInfo().getDatumById(0)->mem_size = 300;
          mcb->s->getThreadInfo().getDatumById(0)-> mem_handle = mcb->mem_mgr->mem_alloc(mcb->s->getThreadInfo().getDatumById(0)->mem_size, 0);
          break;
        }
        case 'Z':
        {
          mcb->s->getThreadInfo().getDatumById(1)->mem_size = 300;
          mcb->s->getThreadInfo().getDatumById(1)-> mem_handle = mcb->mem_mgr->mem_alloc(mcb->s->getThreadInfo().getDatumById(1)->mem_size, 1);
          break;
        }
        case 'x':
        {
          mcb->s->getThreadInfo().getDatumById(2)->mem_size = 300;
          mcb->s->getThreadInfo().getDatumById(2)-> mem_handle = mcb->mem_mgr->mem_alloc(mcb->s->getThreadInfo().getDatumById(2)->mem_size, 2);
          break;
        }
        case 'X':
        {
          mcb->s->getThreadInfo().getDatumById(0)->mem_size = 400;
          mcb->s->getThreadInfo().getDatumById(0)-> mem_handle = mcb->mem_mgr->mem_alloc(mcb->s->getThreadInfo().getDatumById(0)->mem_size, 0);
          break;
        }
      case 'l':
        {
          mcb->mem_mgr->mem_free(mcb->s->getThreadInfo().getDatumById(1)->getMemHandle(),1);
          break;
        }
        case 'L':
        {
          mcb->mem_mgr->mem_free(mcb->s->getThreadInfo().getDatumById(0)->getMemHandle(),0);
          break;
        }
        //Resume5 running
        case 'r':
        {
          mcb->s->resume();
          break;
        }
        case 'R':
        {
          mcb->s->stop();
          break;
        }
        case 'y':
        {
          int tempFileHandle = mcb->ufs->createFile(0,"Big_Daddy",300,0b1100);
          mcb->s->getThreadInfo().getDatumById(0)->fileHandle.addToFront(tempFileHandle,tempFileHandle);
          break;
        }
        case 'Y':
        {
          filewrapperDump(mcb->ufs,mcb->userInf);
          break;
        }
        case 'p':
        {
          filewrapperDumpDir(mcb->ufs,mcb->userInf);
          break;
        }
        case 'P':
        {
          filewrapperDumpDir(mcb->ufs,mcb->userInf, rand()%3);
          break;
        }
        case 'g':
        {
         
          mcb->ufs->writeChar(0,fileOpen1, alphabet[rand()%26],0);
          break;
        }
        case 'G':
        {
          int handle = *mcb->s->getThreadInfo().getDatumById(0)->fileHandle.getNextElement(NULL);
          fileOpen1 =mcb->ufs->openFile(0,handle,"Big_Daddy" , 0b01);
          break;
        }
        
      }
    }
  }

void setMCB(MCB* mcb)
{
  mcb->s->setMCB(mcb);
  mcb->ipc->setMCB(mcb);
  mcb->writeSema->setMCB(mcb);
  mcb->messageSema->setMCB(mcb);
  mcb->mem_mgr->setMCB(mcb);
  mcb->ufs->setMCB(mcb);
}
