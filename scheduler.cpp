// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"

Scheduler::Scheduler() {
  processCount = 0;
}

//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task(void* threadFunction) {
  int createResult;
  int tempOutput[5]; // just for testing

  // create a thread
  createResult = pthread_create(pthread_t[processCount], NULL, &threadFunction, tempOutput[processCount]);
  // check if we ran into issues
  assert(!createResult);

  processCount++;

}

void Scheduler::yield() {
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}
