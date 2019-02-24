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

void* perform_simple_output(void* arguments);


Scheduler::Scheduler()
{
  this->processCount = 0;
  this->tempCounter = 0;
}
//TCB State: 0 running, 1 ready, 2 blocked, 3 dead
//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task(Window* threadWin, Window* headerWin, Window* consoleWin) {

  if(processCount > 5 )
    return;
  int createResult;

  threadInfo[processCount].thread_win = threadWin;
  threadInfo[processCount].head_win = headerWin;
  threadInfo[processCount].console_win = consoleWin;
  threadInfo[processCount].thread_no = processCount;
  TCB tcbTemp;
  tcbTemp.setThreadID(processCount);
  tcbTemp.setState(3);
  tcbTemp.setThreadData(&threadInfo[processCount]);
  TCBList.addToEnd(tcbTemp, processCount);


  // create a thread
  createResult = pthread_create(&pthreads[processCount], NULL, perform_simple_output, &threadInfo[processCount]);
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

void Scheduler::yield()
{
}

void Scheduler::dump(Window* targetWin, int level)
{
  // suspend threads and wait to make sure
  // everything is synced
  char dBuff[255];
  SCHEDULER_SUSPENDED = true;
  TCB* myT = NULL;
  int ts;

  if(level == 3 || level == 4) {
    writeSema.dump(targetWin, level);
  }


  sprintf(dBuff, "\n \n");
  usleep(1000);
  while ((myT = TCBList.getNextElementUntilEnd(myT))) {
    int tn = myT->getThreadData()->getThreadNo();
    int ts = myT->getThreadData()->getState();
    sprintf((dBuff  + strlen(dBuff)), "   Thread: %d \n", tn);
    sprintf((dBuff  + strlen(dBuff)), "      Status: ", tn);
    if (ts == 0)
      sprintf((dBuff  + strlen(dBuff)), "   Running\n");
    else if (ts == 1)
      sprintf((dBuff  + strlen(dBuff)), "   Ready\n");
    else if (ts == 2)
      sprintf((dBuff  + strlen(dBuff)), "   Blocked\n");
    else if (ts == 3)
      sprintf((dBuff  + strlen(dBuff)), "   Dead\n");
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
void* perform_simple_output(void* arguments)
{
  int tempCounter = 0;
  char buff[256];
  int wasLocked = false;
  Scheduler :: thread_data* td = (Scheduler::thread_data*) arguments;

  do {
  while((1) && (td->state != 4))
  {
    // check for suspend called by dump
    while (THREAD_SUSPENDED);

    if(td->state == 0)
    {
      tempCounter++;
      sprintf(buff, "  Task-%d running #%d\n",td->thread_no,tempCounter);
      writeSema.down(td->thread_no);
      td->thread_win->write_window(buff);
      writeSema.up();

      sprintf(buff, "  Thread-%d currently running.\n",td->thread_no);
      writeSema.down(td->thread_no);
      td->console_win->write_window(buff);
      writeSema.up();


      // catch a suspend here in case we
      // didnt get it up top due to timing
      while (THREAD_SUSPENDED);
      // setState() --> replace with YIELD()
      td->state = 1;
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
    if(myT->getThreadData()->getState() == 1)
    {
      myT->getThreadData()->setState(0);
      return (void*) myT;
    }
    //State is blocked or dead don't set running
    return (void*) myT;
  }
  //If last state running is not running
  else if(((TCB*) ID)->getThreadData()->getState() != 0)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    //If next state is blocked or dead don't set to running
    //If next state is ready set to running
    if(myT->getThreadData()->getState() == 1)
    {
      myT->getThreadData()->setState(0);
      return (void*) myT;
    }
    //State is blocked or dead don't set running
    return (void*) myT;
  }

  //Keep running
  return (void*)ID;
}


//linkedList* Scheduler :: getList(){return TCBList;}
