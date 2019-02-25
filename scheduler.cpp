// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <string.h>
#include <assert.h>
#include <fstream>
#include "scheduler.h"
#include "sema.h"
#include <fstream>
#include <mutex>
#include <cmath>
#include <unistd.h> //sleep
#include <random>

Semaphore writeSema("write_window");

bool THREAD_SUSPENDED = false;


Scheduler::Scheduler()
{
  this->processCount = 0;
  this->tempCounter = 0;
  writeSema.retrieveSchedulerObject(this);
}
//TCB State: 0 running, 1 ready, 2 blocked, 3 dead
//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task(Window* threadWin, Window* headerWin, Window* consoleWin) {

  if(processCount > 5 )
    return;

  threadInfo[processCount].thread_win = threadWin;
  threadInfo[processCount].head_win = headerWin;
  threadInfo[processCount].console_win = consoleWin;
  threadInfo[processCount].thread_no = processCount;
  TCB tcbTemp;
  tcbTemp.setThreadID(processCount);
  tcbTemp.setState(1);
  tcbTemp.setThreadData(&threadInfo[processCount]);
  TCBList.addToEnd(tcbTemp, processCount);
  int createResult;

  // create a thread
  typedef void * (*TP)(void *);
  createResult = pthread_create(&pthreads[processCount], NULL, (TP)&Scheduler::perform_simple_output, this);
  //threadInfo[0].thread_win->display_help();
  // wait for termination and check if we ran into issues
  // createResult = pthread_join(pthreads[processCount], NULL);
  assert(!createResult);
  char buff[256];
  sprintf(buff, " Thread-%d created.\n",threadInfo[processCount].thread_no);
  threadInfo[processCount].thread_win->write_window("\n");
  threadInfo[processCount].head_win->write_window(processCount + 1, 1,buff);
  //dump(5);
  processCount++;

}

void Scheduler::yield(int threadNum)
{
  TCBList.getDatumById(threadNum)->setState(1);
}

void Scheduler::dump(Window* targetWin, int level)
{
  // suspend threads and wait to make sure
  // everything is synced
  char dBuff[255];
  SCHEDULER_SUSPENDED = true;
  TCB* myT = NULL;

  if(level == 3 || level == 4) {
    writeSema.dump(targetWin, level);
  }

  else
  {
      sprintf(dBuff, "\n \n");
      usleep(5000);
      while ((myT = TCBList.getNextElementUntilEnd(myT))) {
        int tn = myT->getThreadID();
        int ts = myT->getState();
        sprintf((dBuff  + strlen(dBuff)), "   Thread: %d \n", tn);
        sprintf((dBuff  + strlen(dBuff)), "      Status: ");
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

// stop():   This function stops all operations of the program
// input(s): None
// returns:  void
void Scheduler::stop() {
  THREAD_SUSPENDED = true;
}

void Scheduler::resume() {
  THREAD_SUSPENDED = false;
}

//thread worker function

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

// running():
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

// garbage_collect(): Finds all terminated tasks and sets them to
// state 4 which will end their while loops and the thread dies
void Scheduler::garbage_collect() {
  TCB* myT = NULL;
  while ((myT = TCBList.getNextElementUntilEnd(myT))) {
    if(myT->getState() == 3) {
      myT->setState(4);
    }
  }
}


//linkedList* Scheduler :: getList(){return TCBList;}
