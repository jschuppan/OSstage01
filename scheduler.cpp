/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : scheduler.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of scheduler.h
============================================================================*/

//=================================
// necessary includes
#include <pthread.h>
#include <string.h>
#include <assert.h>
//#include <mutex>
#include <unistd.h> //sleep
#include <random>
#include <fstream>
#include "scheduler.h"
#include "sema.h"
#include "IPC.h"
#include "UI.h"
#include <stdlib.h>
#include "mem_mgr.h"

//Global Variables
const int RUNNING = 0;
const int READY = 1;
const int BLOCKED = 2;
const int KILLED = 4;
const int DEAD = 3;

//defualt constructor
Scheduler::Scheduler()
{
  this->processCount = 0;
  this->tempCounter = 0;
  this->SCHEDULER_SUSPENDED = false;
  this->THREAD_SUSPENDED = false;
  this->SCHEDULER_COMPLETED_RUN = false;
  this->mcb = NULL;
  this->force_Deadlock = false;
}

/*-----------------------------------------------------------------
Function      : create_task(Window* threadWin, Window* headerWin, Window* consoleWin);
Parameters    : 3 Window ptrs
Returns       : void
Details       : TCB State: 0 running, 1 ready, 2 blocked, 3 dead
                creates a thread and assigns the windows to the threads
                arguments
------------------------------------------------------------------*/
void Scheduler::create_task(Window* threadWin, Window* headerWin, Window* consoleWin) {

  if(processCount > 5 )
    return;

  thread_data threadData;
  threadData.thread_win = threadWin;
  threadData.head_win = headerWin;
  threadData.console_win = consoleWin;
  threadData.thread_no = processCount;
  //Set thread data
  threadInfo.addToEnd(threadData, processCount);

  TCB tcbTemp;
  //set TCB data
  tcbTemp.setThreadID(processCount);
  tcbTemp.setState(READY);
  tcbTemp.setThreadData(threadData);
  TCBList.addToEnd(tcbTemp, processCount);
  mcb->ipc->createMailbox(processCount);
  int createResult;

  // create a thread
  typedef void * (*TP)(void *);
  pthread_t Pt;
  pthreads.addToEnd(Pt, processCount);
  createResult = pthread_create(pthreads.getDatumById(processCount), NULL, (TP)&Scheduler::perform_simple_output, this);
  // wait for termination and check if we ran into issues
  assert(!createResult);
  char buff[256];
  mcb->messageSema->down(processCount);
  sprintf(buff, " Thread-%d created.\n",threadInfo.getDatumById(processCount)->thread_no);
  threadInfo.getDatumById(processCount)->thread_win->write_window("\n");
  threadInfo.getDatumById(processCount)->head_win->write_window(processCount + 1, 1,buff);
  mcb->messageSema ->up();


  processCount++;

}

/*-----------------------------------------------------------------
Function      : yield(int threadNum);
Parameters    : int thread id
Returns       : void
Details       : changes state of thread to ready
------------------------------------------------------------------*/
void Scheduler::yield(int threadNum)
{
  TCBList.getDatumById(threadNum)->setState(1);
}

/*-----------------------------------------------------------------
Function      : dump(Window* targetWin, int level);
Parameters    : ptr to Window object, int level to be dumped
Returns       : void
Details       : dumps thread infor out to a screen
------------------------------------------------------------------*/
void Scheduler::dump(Window* targetWin, int level)
{
  // suspend threads and wait to make sure
  // everything is synced
  char dBuff[255];
  //while(!SCHEDULER_COMPLETED_RUN);
  SCHEDULER_SUSPENDED = true;
  TCB* myT = NULL;


  //use Scheduler dump level 1
  if(level == 1)
  {
      sprintf(dBuff, "\n \n   ThreadNum \t State ");
      usleep(5000);
      targetWin->write_window(dBuff);

      //loop until end of list
      while ((myT = TCBList.getNextElementUntilEnd(myT)))
      {
        int tn = myT->getThreadID();
        int ts = myT->getState();
        sprintf(dBuff, "\n");
        sprintf((dBuff  + strlen(dBuff)), "   %d \t\t", tn);

        //Check status of thread and output corresponing value
        if (ts == RUNNING)
          sprintf((dBuff  + strlen(dBuff)), " Running\t");
        else if (ts == READY)
          sprintf((dBuff  + strlen(dBuff)), " Ready\t\t");
        else if (ts == BLOCKED)
          sprintf((dBuff  + strlen(dBuff)), " Blocked\t\t");
        else if (ts == DEAD)
          sprintf((dBuff  + strlen(dBuff)), " Dead\t\t");
        else if (ts == KILLED)
          sprintf((dBuff  + strlen(dBuff)), " Killed\t\t");
        targetWin->write_window(dBuff);
      }
  }
  else if(level == 2)
  {
    sprintf(dBuff, "\n \n   Thread_Num \t State \t\t Window_Name \t MessageCount ");
    usleep(5000);
    targetWin->write_window(dBuff);

    //loop until end of list
    while ((myT = TCBList.getNextElementUntilEnd(myT)))
    {
      int tn = myT->getThreadID();
      int ts = myT->getState();
      sprintf(dBuff, "\n");
      sprintf((dBuff  + strlen(dBuff)), "   %d \t\t", tn);

      //Check status of thread and output corresponing value
      if (ts == RUNNING)
        sprintf((dBuff  + strlen(dBuff)), " Running\t");
      else if (ts == READY)
        sprintf((dBuff  + strlen(dBuff)), " Ready\t\t");
      else if (ts == BLOCKED)
        sprintf((dBuff  + strlen(dBuff)), " Blocked\t");
      else if (ts == DEAD)
        sprintf((dBuff  + strlen(dBuff)), " Dead\t\t");
      else if (ts == KILLED)
        sprintf((dBuff  + strlen(dBuff)), " Killed\t\t");

      //Additional print from level 1 follows
      std::string str = "ThreadWin ";
      char* chr = strdup(str.c_str());

      snprintf((dBuff + strlen(dBuff)),sizeof(dBuff), "%s %d", chr, tn);
      free(chr);
      //Number of messages
      sprintf(dBuff + strlen(dBuff), "\t\t %d", mcb->ipc->threadMailboxes.getDatumById(tn)->getSize());
      targetWin->write_window(dBuff);
    }
  }
  //use Semaphore dump
  else
  {
    mcb->writeSema->dump(targetWin, level);
    mcb->messageSema->dump(targetWin, level);
  }
  SCHEDULER_SUSPENDED = false;
}

//=======================================
//This is used to test Semaphore
//=======================================
void Scheduler::forceWrite(int threadNum)
{
  mcb->writeSema->down(threadNum);
  //writeSema.up();
}

/*-----------------------------------------------------------------
Function      : messageDump(Window* targetWin, int level);
Parameters    : ptr to Window object, int level to be dumped
Returns       : void
Details       : dumps message info out to a screen
------------------------------------------------------------------*/
void Scheduler::messageDump(Window* targetWin, int level)
{
  char mBuff[16384];
  std::string tempString;
  char* chr;
  SCHEDULER_SUSPENDED = true;

  // get threadID of current element
  tempString = mcb->ipc->Message_Print();

  // store returned string into buffer
  chr = strdup(tempString.c_str());
  sprintf(mBuff, "%s",chr);

  // write buffer to window
  usleep(5000);
  targetWin->write_window(mBuff);
  // deallocate memory for chr
  free(chr);

  SCHEDULER_SUSPENDED = false;

}


/*-----------------------------------------------------------------
Function      : stop();
Parameters    :
Returns       : void
Details       : stops all threads
------------------------------------------------------------------*/
void Scheduler::stop() {
  THREAD_SUSPENDED = true;
}

/*-----------------------------------------------------------------
Function      : resume();
Parameters    :
Returns       : void
Details       : restarts all threads
------------------------------------------------------------------*/
void Scheduler::resume() {
  THREAD_SUSPENDED = false;
}

/*-----------------------------------------------------------------
Function      : perform_simple_output(void* arguments);
Parameters    : void* arguments
Returns       : void*
Details       : Thread worker function,
                Thread self yeilds after one cycle is complete
------------------------------------------------------------------*/
void* Scheduler::perform_simple_output(void* arguments)
{
  int tempCounter = 0;
  char buff[256];
  int i = 0;
  int count = -1;


  // find out who we are
  while(*pthreads.getDatumById(i) != pthread_self()) {
    i++;
  }
  int threadNum = i;

    // run in endless loop until killed by garbage_collect()
    while((1) && (TCBList.getDatumById(threadNum)->getState() != DEAD))
    {
      // check for suspend called by dump
      while (THREAD_SUSPENDED);

      if(TCBList.getDatumById(threadNum)->getState() == RUNNING)
      {
        usleep(200000);
        tempCounter++;
        sprintf(buff, "  Task-%d running #%d\n",threadNum,tempCounter);
        mcb->writeSema->down(threadNum);
        threadInfo.getDatumById(threadNum)->getThreadWin()->write_window(buff);
        mcb->writeSema->up();

        sprintf(buff, "  Thread-%d currently running.\n",threadNum);
        mcb->writeSema->down(threadNum);
        threadInfo.getDatumById(threadNum)->getConsoleWin()->write_window(buff);
        // td->console_win->write_window(buff);
        if(!force_Deadlock)
          mcb->writeSema->up();
        // catch a suspend here in case we
        // didnt get it up top due to timing
        while (THREAD_SUSPENDED);
        //process yields itself after completing run
          yield(threadNum);
          count++;
      }
      else {
        pthread_yield();
      }
    }

    // if thread is about to die lets make sure its mailbox gets cleared out
    mcb->ipc->Message_DeleteAll(threadNum);
}

/*-----------------------------------------------------------------
Function      : running(void* ID);
Parameters    : void* ID
Returns       : void* (TCB)
Details       : checks if the last thread that was running is
                done running and sets the next thread to running.
                If it is not done running let it run
------------------------------------------------------------------*/
void* Scheduler:: running(void* ID)
{
  SCHEDULER_COMPLETED_RUN = false;

  //If last running is NULL
  if((TCB*) ID == NULL)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    //If next state is ready set to running
    if(myT->getState() == READY)
    {
      myT->setState(RUNNING);
      return (void*) myT;
    }
    //State is blocked or dead don't set running
    return (void*) myT;
  }
  //If last state running is not running
  else if(((TCB*) ID)->getState() != RUNNING)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    //If next state is blocked or dead don't set to running
    //If next state is ready set to running
    if(myT->getState() == READY)
    {
      myT->setState(RUNNING);
      return (void*) myT;
    }
    //State is blocked or dead don't set running
    return (void*) myT;
  }

  SCHEDULER_COMPLETED_RUN = true;
  //Keep running
  return (void*)ID;
}

/*-----------------------------------------------------------------
Function      : garbage_collect();
Parameters    :
Returns       : void
Details       : Finds all terminated tasks and sets them to
                state 4 which will end their while loops and
                the thread dies
------------------------------------------------------------------*/
void Scheduler::garbage_collect() {
  TCB* myT = NULL;
  TCB* temp = myT;
  myT = TCBList.getNextElementUntilEnd(myT);

  while (myT)
  {
    if(myT->getState() == DEAD)
    {
      temp = myT;
      stop();
      myT->setState(KILLED);
      //TCBList.removeNodeByElement(temp->getThreadID());
    }
      myT = TCBList.getNextElementUntilEnd(myT);
  }
}

/*-----------------------------------------------------------------
Function      : setMCB()
Parameters    :
Returns       : void
Details       : sets the mcb
--------------------+++++++----------------------------------------------*/
void Scheduler::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}
