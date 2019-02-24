// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
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
  processCount = 0;
  tempCounter = 0;
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
  tcbTemp.setState(1);
  tcbTemp.setThreadData(&threadInfo[processCount]);
  TCBList.addToFront(tcbTemp, processCount);


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

void Scheduler::dump(WINDOW* targetWin, int level)
{
  // suspend threads and wait to make sure
  // everything is synced
  stop();
  SCHEDULER_SUSPENDED = true;


  int bs = 0;
  debugDump.open("sched_dump.txt", std:: ofstream::app);
  debugDump << "\n DUMP START \n";
  // threadStop
  while((TCBList.getDatumById(bs)) != NULL) {
    debugDump << "---- \n ThreadID: " << bs << std::endl
               << "   " << "Status: "
               << TCBList.getDatumById(bs)->getThreadData()->getState()
               << std::endl << "---- \n";
    bs++;
  }
  debugDump  << "\n DUMP END";

  SCHEDULER_SUSPENDED = false;
  resume();
  debugDump.close();
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

void* perform_simple_output(void* arguments)
{
  int tempCounter = 0;
  char buff[256];
  int wasLocked = false;
  Scheduler :: thread_data* td = (Scheduler::thread_data*) arguments;
  std::ofstream threadDebug;
  threadDebug.open("threadStatus.txt");

  do {
  while((1) && (td->state != 4))
  {
    // check for suspend called by dump
    while (THREAD_SUSPENDED);

    threadDebug << td->thread_no << " : " <<  td->state << std::endl;

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
  threadDebug.close();

}

// running():
void* Scheduler:: running(void* ID)
{
  if((TCB*) ID == NULL)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    myT->getThreadData()->setState(0);
    return (void*) myT;
  }
  else if(((TCB*) ID)->getThreadData()->getState() != 0)
  {
    TCB* myT = TCBList.getNextElement((TCB*)ID);
    myT->getThreadData()->setState(0);
    return (void*) myT;
  }

  return (void*)ID;
}


//linkedList* Scheduler :: getList(){return TCBList;}
