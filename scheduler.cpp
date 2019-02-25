/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : scheduler.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of scheduler.h
============================================================================*/
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "scheduler.h"
#include "sema.h"
#include <mutex>
#include <unistd.h> //sleep
#include <random>

//Global Variables
Semaphore writeSema("write_window");
bool THREAD_SUSPENDED = false;

//defualt constructor
Scheduler::Scheduler()
{
  this->processCount = 0;
  this->tempCounter = 0;
  writeSema.retrieveSchedulerObject(this);
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
  //Set thread data
  threadInfo[processCount].thread_win = threadWin;
  threadInfo[processCount].head_win = headerWin;
  threadInfo[processCount].console_win = consoleWin;
  threadInfo[processCount].thread_no = processCount;
  TCB tcbTemp;
  //set TCB data
  tcbTemp.setThreadID(processCount);
  tcbTemp.setState(1);
  tcbTemp.setThreadData(&threadInfo[processCount]);
  TCBList.addToEnd(tcbTemp, processCount);
  int createResult;

  // create a thread
  typedef void * (*TP)(void *);
  createResult = pthread_create(&pthreads[processCount], NULL, (TP)&Scheduler::perform_simple_output, this);
  // wait for termination and check if we ran into issues
  assert(!createResult);
  char buff[256];
  sprintf(buff, " Thread-%d created.\n",threadInfo[processCount].thread_no);
  threadInfo[processCount].thread_win->write_window("\n");
  threadInfo[processCount].head_win->write_window(processCount + 1, 1,buff);

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
  SCHEDULER_SUSPENDED = true;
  TCB* myT = NULL;

  //use semaphore dump
  if(level == 3 || level == 4) {
    writeSema.dump(targetWin, level);
  }

  //use Scheduler dump
  else
  {
      sprintf(dBuff, "\n \n");
      usleep(5000);
      //loop until end of list
      while ((myT = TCBList.getNextElementUntilEnd(myT))) {
        int tn = myT->getThreadID();
        int ts = myT->getState();
        sprintf((dBuff  + strlen(dBuff)), "   Thread: %d \n", tn);
        sprintf((dBuff  + strlen(dBuff)), "      Status: ");
        //Check status of thread and output corresponing value
        if (ts == 0)
          sprintf((dBuff  + strlen(dBuff)), "   Running\n");
        else if (ts == 1)
          sprintf((dBuff  + strlen(dBuff)), "   Ready\n");
        else if (ts == 2)
          sprintf((dBuff  + strlen(dBuff)), "   Blocked\n");
        else if (ts == 3)
          sprintf((dBuff  + strlen(dBuff)), "   Dead\n");
        else if (ts == 4)
          sprintf((dBuff  + strlen(dBuff)), "   Killed (Thread terminated)\n");
  }
}

  targetWin->write_window(dBuff);

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
  // find out who we are
  while(pthreads[i] != pthread_self()) {
    i++;
  }
  int threadNum = i;

  do {
    // run in endless loop until killed by garbage_collect()
    while((1) && (TCBList.getDatumById(threadNum)->getState() != 4))
    {
      // check for suspend called by dump
      while (THREAD_SUSPENDED);

      if(TCBList.getDatumById(threadNum)->getState() == 0)
      {
        tempCounter++;
        sprintf(buff, "  Task-%d running #%d\n",threadNum,tempCounter);
        writeSema.down(threadNum);
        threadInfo[threadNum].getThreadWin()->write_window(buff);
        // td->thread_win->write_window(buff);
        writeSema.up();

        sprintf(buff, "  Thread-%d currently running.\n",threadNum);
        writeSema.down(threadNum);
        threadInfo[threadNum].getConsoleWin()->write_window(buff);
        // td->console_win->write_window(buff);
        writeSema.up();


        // catch a suspend here in case we
        // didnt get it up top due to timing
        while (THREAD_SUSPENDED);

        // process yields itself after completing run
        yield(threadNum);
      }
      else {
        pthread_yield();
      }
    }
  } while(!THREAD_SUSPENDED);
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
  //If last running is NULL
  if((TCB*) ID == NULL)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    //If next state is ready set to running
    if(myT->getState() == 1)
    {
      myT->setState(0);
      return (void*) myT;
    }
    //State is blocked or dead don't set running
    return (void*) myT;
  }
  //If last state running is not running
  else if(((TCB*) ID)->getState() != 0)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    //If next state is blocked or dead don't set to running
    //If next state is ready set to running
    if(myT->getState() == 1)
    {
      myT->setState(0);
      return (void*) myT;
    }
    //State is blocked or dead don't set running
    return (void*) myT;
  }

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
  while ((myT = TCBList.getNextElementUntilEnd(myT))) {
    if(myT->getState() == 3) {
      myT->setState(4);
    }
  }
}
