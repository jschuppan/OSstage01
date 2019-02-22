// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"

void Scheduler::create_task(functionPtr, threadArg, threadName) {
  int createResult;

  createResult =
  assert(!createResult);

}

void Scheduler::yield() {
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}
