// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"


Scheduler::Scheduler()
{
  processCount = 0;
  tempCounter = 0;
}

//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task(Window* Win) {
  int createResult;
  threadInfo[processCount].thread_win = Win;
  // create a thread
  createResult = pthread_create(&pthreads[processCount], NULL, (THREADFUNCPTR) &Scheduler::perform_simple_output, &threadInfo[processCount]);
  // check if we ran into issues
  //threadInfo[processCount].thread_win->write_window(1,1,"Hello");
  createResult = pthread_join(pthreads[processCount], NULL);

  //assert(!createResult);

  processCount++;

}

void Scheduler::yield()
{
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}

void* Scheduler :: perform_simple_output(void* arguments)
{
  for (int i=0; i < 10000; i++) {
    tempCounter += i;
    thread_win->write_window(1,1,tempCounter);
  }
}
