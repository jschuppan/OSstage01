// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"
#include <fstream>

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

  // wait for termination and check if we ran into issues
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
    Window* twindow = threadInfo[0].thread_win;
    // std::ofstream debugFile2;
    // debugFile2.open("debug_thread.txt");
    // debugFile2 << "twindow" << twindow <<"\n";
    // debugFile2.close();



    //// the issue with the core dump is related to how the window
    //// ptr is passed
    twindow->write_window(1,1, "test");
  }
}
